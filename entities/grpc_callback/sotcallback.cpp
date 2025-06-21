#include "sotcallback.h"

#include <QThread>

class SotDataLister : public grpc::ServerWriteReactor<core_ips::sot::TrackResponse> {
   public:
    SotDataLister(SotCallback* sotCallback, grpc::CallbackServerContext* context)
        : _startingTimepoint(std::chrono::steady_clock::now()),
          _sotLostTrackCounter(0),
          _sotCallback(sotCallback),
          _finishStatus(grpc::StatusCode::UNKNOWN, ""),
          _context(context) {
        if (_sotCallback) {
            _streamTimeoutMsecs = _sotCallback->_streamTimeoutMsecs;
            _sotCallback->_isBusy.store(true);
        }
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
        std::cout << "[SotDataLister] OnDone" << std::endl;
        if (_sotCallback && _sotCallback->grpcServer()) {
            QMetaObject::invokeMethod(_sotCallback->grpcServer(),
                                      &GrpcServer::hasSotTrackStop,
                                      Qt::QueuedConnection);
        }
        delete this;
    }

    void OnCancel() override {
        std::cout << "[SotDataLister] OnCancel" << std::endl;
    }

   private:
    void _finish() {
        Finish(_finishStatus);
    }

    bool _readDataFromQueueAndWriteToStream() {
        if (!_sotCallback) return false;
        bool result{false};
        for (;;) {
            if (_context->IsCancelled() || !_sotCallback->_isBusy.load()) {
                _finishStatus = grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "Has new incoming request or cancellation signal");
                result = false;
                break;
            }

            if (_sotLostTrackCounter > 10) {  // TODO: must read from config file
                _finishStatus = grpc::Status(grpc::StatusCode::CANCELLED, "Over lost tracking");
                result = false;
                break;
            }

            sot::SotInfo incomingInfo;
            bool isRead = _sotCallback->_dataQueue.popFront(incomingInfo);
            if (isRead) {
                _startingTimepoint = std::chrono::steady_clock::now();
                if (incomingInfo.score > 0.9f) {
                    _sotLostTrackCounter = 0;
                } else {
                    _sotLostTrackCounter++;
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
                if (curDuration >= _streamTimeoutMsecs) {
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
    int _sotLostTrackCounter;
    int _writeNotDoneCounter;
    grpc::Status _finishStatus;
    grpc::CallbackServerContext* _context;
    core_ips::sot::TrackResponse _output;
    std::chrono::milliseconds _streamTimeoutMsecs;
    std::chrono::steady_clock::time_point _startingTimepoint;
};

SotCallback::SotCallback(GrpcServer* grpcServer)
    : CallbackBase(grpcServer),
      _isBusy(false),
      _dataQueue(SafeQueue<sot::SotInfo>()),
      _streamTimeoutMsecs(5000) {  // TODO: Must be read from a config file
}

SotCallback::~SotCallback() {
}

grpc::ServerWriteReactor<core_ips::sot::TrackResponse>* SotCallback::Track(
    grpc::CallbackServerContext* context,
    const core_ips::sot::TrackRequest* request) {
    if (_isBusy.load()) {
        _isBusy.store(false);
    }

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
    }
    _dataQueue.clear();
    return new SotDataLister(this, context);
}

void SotCallback::pushResultData(const sot::SotInfo& info) {
    _dataQueue.pushBack(info);
}