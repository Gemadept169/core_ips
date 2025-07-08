#include "sotcallback.h"

#include <QMetaObject>
#include <QThread>

#include "utilities/logger.h"

class TrackStartImpl : public grpc::ServerWriteReactor<core_ips::sot::TrackResponse> {
   public:
    TrackStartImpl(SotCallback* sotCallback, grpc::CallbackServerContext* context)
        : _startingTimepoint(std::chrono::steady_clock::now()),
          _trackLostFrameCounter(0),
          _sotCallback(sotCallback),
          _finishStatus(grpc::StatusCode::UNKNOWN, ""),
          _context(context) {
        _writerTimeoutMsecs = _sotCallback->_writerTimeoutMsecs;
        _trackLostFrameMax = _sotCallback->_trackLostFrameMax;
        _sotCallback->_isBusy.store(true);
        if (!_readDataFromQueueAndWriteToStream()) {
            _finish();
        }
    }

    void OnWriteDone(bool ok) override {
        if (ok) {
            if (!_readDataFromQueueAndWriteToStream()) {
                _finish();
            }
        } else {
            _finishStatus = grpc::Status(grpc::StatusCode::UNKNOWN, "Unexpected failure when write done");
            _finish();
        }
    }

    void OnDone() override {
        if (_sotCallback->grpcServer()) {
            QMetaObject::invokeMethod(_sotCallback->grpcServer(),
                                      &GrpcServer::hasSotTrackStop,
                                      Qt::QueuedConnection);
            LOG_DEBUG("TrackStartImpl: OnDone func sent a signal to stop task");
        }
        {
            std::unique_lock<std::mutex> lock(_sotCallback->_mu);
            _sotCallback->_cv.notify_one();
            LOG_DEBUG("TrackStartImpl: OnDone func notified one");
        }
        _sotCallback->_isBusy.store(false);
        delete this;
    }

    void OnCancel() override {
        LOG_DEBUG("TrackStartImpl: OnCancel func");
    }

   private:
    void _finish() {
        Finish(_finishStatus);
    }

    bool _readDataFromQueueAndWriteToStream() {
        bool result = false;
        for (;;) {
            if (_context->IsCancelled()) {
                _finishStatus = grpc::Status(grpc::StatusCode::CANCELLED, "Cancellation signal from context");
                result = false;
                break;
            }
            if (!_sotCallback->_isBusy.load()) {
                _finishStatus = grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "New incoming request");
                result = false;
                break;
            }
            if (!_sotCallback->_isVideoConnected.load()) {
                _finishStatus = grpc::Status(grpc::StatusCode::CANCELLED, "Video streaming is disconnected");
                result = false;
                break;
            }
            if (_trackLostFrameCounter >= _trackLostFrameMax) {
                _finishStatus = grpc::Status(grpc::StatusCode::CANCELLED, "Over lost tracking");
                result = false;
                break;
            }

            sot::SotInfo incomingInfo;
            bool isRead = _sotCallback->_dataQueue.popFront(incomingInfo);
            if (isRead) {
                _startingTimepoint = std::chrono::steady_clock::now();
                if (incomingInfo.score > 0.9f) {
                    _trackLostFrameCounter = 0;
                } else {
                    _trackLostFrameCounter++;
                }
                // Refer: https://stackoverflow.com/questions/33960999/protobuf-will-set-allocated-delete-the-allocated-object
                auto out = new core_ips::sot::SotInfo();
                auto outBox = new core_ips::sot::BBox();
                outBox->set_xtl(incomingInfo.bbox.xtl);
                outBox->set_ytl(incomingInfo.bbox.ytl);
                outBox->set_width(incomingInfo.bbox.width);
                outBox->set_height(incomingInfo.bbox.height);
                out->set_allocated_bbox(outBox);
                out->set_score(incomingInfo.score);
                _output.set_allocated_result(out);
                _output.set_state(core_ips::sot::TrackResponse::State::TrackResponse_State_NORMAL);
                StartWrite(&_output);
                result = true;
                break;
            } else {
                auto curTimepoint = std::chrono::steady_clock::now();
                std::chrono::milliseconds curDuration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(curTimepoint - _startingTimepoint);
                if (curDuration >= _writerTimeoutMsecs) {
                    _finishStatus = grpc::Status(grpc::StatusCode::CANCELLED, "Over processing timeout");
                    result = false;
                    break;
                } else {
                    continue;
                }
            }
        }
        return result;
    }

    SotCallback* _sotCallback;
    int _trackLostFrameCounter;
    int _trackLostFrameMax;
    grpc::Status _finishStatus;
    grpc::CallbackServerContext* _context;
    core_ips::sot::TrackResponse _output;
    std::chrono::milliseconds _writerTimeoutMsecs;
    std::chrono::steady_clock::time_point _startingTimepoint;
};

SotCallback::SotCallback(GrpcServer* grpcServer,
                         const std::chrono::milliseconds& writerTimeoutMsecs,
                         const unsigned int& trackLostFrameMax)
    : CallbackBase(grpcServer),
      _isBusy(false),
      _isVideoConnected(true),
      _dataQueue(SafeQueue<sot::SotInfo>()),
      _writerTimeoutMsecs(writerTimeoutMsecs),
      _trackLostFrameMax(trackLostFrameMax) {
}

SotCallback::~SotCallback() {
}

grpc::ServerWriteReactor<core_ips::sot::TrackResponse>*
SotCallback::TrackStart(grpc::CallbackServerContext* context,
                        const core_ips::sot::TrackRequest* request) {
    LOG_DEBUG("SotCallback: TrackStart has new request");
    if (_isBusy.load()) {
        _isBusy.store(false);
        std::unique_lock<std::mutex> lock(_mu);
        _cv.wait(lock);
    }
    LOG_DEBUG("SotCallback: TrackStart deleted old task");
    if (grpcServer() && request != nullptr) {
        sot::BBox incomingBox;
        incomingBox.xtl = request->init_bbox().xtl();
        incomingBox.ytl = request->init_bbox().ytl();
        incomingBox.width = request->init_bbox().width();
        incomingBox.height = request->init_bbox().height();
        QMetaObject::invokeMethod(grpcServer(),
                                  &GrpcServer::hasSotTrackNewRequest,
                                  Qt::QueuedConnection,
                                  incomingBox);
        LOG_DEBUG("SotCallback: TrackStart sent a signal to init new sot task");
    }
    _dataQueue.clear();
    return new TrackStartImpl(this, context);
}

grpc::ServerUnaryReactor* SotCallback::TrackStop(grpc::CallbackServerContext* context,
                                                 const google::protobuf::Empty* request,
                                                 google::protobuf::Empty* response) {
    LOG_DEBUG("SotCallback: TrackStop has new request");
    if (_isBusy.load()) {
        _isBusy.store(false);
    }

    if (grpcServer()) {
        QMetaObject::invokeMethod(grpcServer(),
                                  &GrpcServer::hasSotTrackStop,
                                  Qt::QueuedConnection);
    }

    auto* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}

void SotCallback::pushResultData(const sot::SotInfo& info) {
    _dataQueue.pushBack(info);
}

void SotCallback::setIsVideoConnected(const bool& isConnected) {
    if (_isVideoConnected.load() != isConnected) {
        _isVideoConnected.store(isConnected);
    }
}

void SotCallback::setTrackLostFrameMax(const unsigned int& frameMax) {
    _trackLostFrameMax = frameMax;
}

void SotCallback::setWriterTimeoutMsecs(const unsigned int& msec) {
    _writerTimeoutMsecs = std::chrono::milliseconds(msec);
}
