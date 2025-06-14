#ifndef GRPC_CALLBACK_SOTCALLBACK_H
#define GRPC_CALLBACK_SOTCALLBACK_H

#include "callbackbase.h"
#include "sot/types.hpp"
#include "sot_service.grpc.pb.h"
#include "utilities/safequeue.h"

class SotDataLister;
class SotCallback : public CallbackBase,
                    public core_ips::sot::Sot::CallbackService {
    friend SotDataLister;

   public:
    explicit SotCallback(GrpcServer* grpcServer);
    SotCallback(const SotCallback&) = delete;
    SotCallback& operator=(const SotCallback&) = delete;
    ~SotCallback();

    grpc::ServerWriteReactor<::core_ips::sot::TrackResponse>* Track(grpc::CallbackServerContext* context,
                                                                    const ::core_ips::sot::TrackRequest* request) override;

   private:
    SafeQueue<::sot::SotInfo> _dataQueue;
    std::chrono::milliseconds _streamTimeoutMsecs;
};

#endif