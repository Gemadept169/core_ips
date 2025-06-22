#ifndef GRPC_CALLBACK_SOTCALLBACK_H
#define GRPC_CALLBACK_SOTCALLBACK_H

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "callbackbase.h"
#include "sot/types.hpp"
#include "sot_service.grpc.pb.h"
#include "utilities/safequeue.h"

class TrackStartImpl;
class SotCallback : public CallbackBase,
                    public core_ips::sot::Sot::CallbackService {
    friend TrackStartImpl;

   public:
    explicit SotCallback(GrpcServer* grpcServer,
                         const std::chrono::milliseconds& writerTimeoutMsecs = std::chrono::milliseconds(1000),
                         const unsigned int& trackLostFrameMax = 20);
    SotCallback(const SotCallback&) = delete;
    SotCallback& operator=(const SotCallback&) = delete;
    ~SotCallback();

    grpc::ServerWriteReactor<core_ips::sot::TrackResponse>* TrackStart(grpc::CallbackServerContext* context,
                                                                       const core_ips::sot::TrackRequest* request) override;

    grpc::ServerUnaryReactor* TrackStop(grpc::CallbackServerContext* context,
                                        const google::protobuf::Empty* request,
                                        google::protobuf::Empty* response) override;

    void pushResultData(const sot::SotInfo& info);

   private:
    std::atomic_bool _isBusy;
    std::condition_variable _cv;
    std::mutex _mu;
    SafeQueue<sot::SotInfo> _dataQueue;
    std::chrono::milliseconds _writerTimeoutMsecs;
    unsigned int _trackLostFrameMax;
};

#endif