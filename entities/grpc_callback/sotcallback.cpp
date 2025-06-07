#include "sotcallback.h"

class SotDataLister : public grpc::ServerWriteReactor<::core_ips::sot::TrackResponse> {
   public:
    SotDataLister(SotCallback* sotCallback)
        : _startingTimepoint(std::chrono::steady_clock::now()),
          _sotCallback(sotCallback) {
        if (_sotCallback) {
            _streamTimeoutMsecs = _sotCallback->_streamTimeoutMsecs;
        }
        if (!readDataFromQueueAndWriteToStream()) {
            Finish(grpc::Status(grpc::StatusCode::CANCELLED, "Failed at SotDataLister constructor"));
        }
    }

    void OnWriteDone(bool ok) override {
        if (ok) {
            if (!readDataFromQueueAndWriteToStream()) {
                Finish(grpc::Status(grpc::StatusCode::CANCELLED, "Sot data stream timeout was reached or has a terminated signal"));
            }
        } else {
            Finish(grpc::Status(grpc::StatusCode::UNKNOWN, "Unexpected Failure"));
        }
    }

    void OnDone() override {
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
    bool readDataFromQueueAndWriteToStream() {
        if (!_sotCallback) return false;
        bool result{false};
        for (;;) {
            sot::SotInfo incomingInfo;
            bool isRead = _sotCallback->_dataQueue.popFrontBlockingWithTimeout(incomingInfo);
            if (isRead) {
                _startingTimepoint = std::chrono::steady_clock::now();
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
                _output.set_state(::core_ips::sot::TrackResponse::State::TrackResponse_State_NORMAL);
                StartWrite(&_output);
                result = true;
                break;
            } else {
                auto curTimepoint = std::chrono::steady_clock::now();
                std::chrono::milliseconds curDuration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(curTimepoint - _startingTimepoint);
                std::cout << "+== duration:" << curDuration.count() << std::endl;
                if (curDuration >= _streamTimeoutMsecs) {
                    result = false;
                    break;
                } else {
                    continue;
                }
            }
        }
        return result;
    }

    ::core_ips::sot::TrackResponse _output;
    std::chrono::milliseconds _streamTimeoutMsecs;
    SotCallback* _sotCallback;
    std::chrono::steady_clock::time_point _startingTimepoint;
};

SotCallback::SotCallback(GrpcServer* grpcServer)
    : CallbackBase(grpcServer),
      _dataQueue(SafeQueue<::sot::SotInfo>(1000)),  // TODO: Must be read from a config file
      _streamTimeoutMsecs(5000) {                   // TODO: Must be read from a config file
}

SotCallback::~SotCallback() {
}

grpc::ServerWriteReactor<::core_ips::sot::TrackResponse>* SotCallback::Track(
    grpc::CallbackServerContext* context,
    const core_ips::sot::TrackRequest* request) {
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
    return new SotDataLister(this);
}