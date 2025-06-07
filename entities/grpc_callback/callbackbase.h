#ifndef GRPC_CALLBACK_CALLBACKBASE_H
#define GRPC_CALLBACK_CALLBACKBASE_H

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <QMetaObject>
#include <QPointer>

#include "controllers/grpcserver.h"

class CallbackBase {
   public:
    explicit CallbackBase(GrpcServer* grpcServer);
    CallbackBase(const CallbackBase&) = delete;
    CallbackBase& operator=(const CallbackBase&) = delete;
    ~CallbackBase();

    QPointer<GrpcServer> grpcServer() const;

   protected:
    QPointer<GrpcServer> _grpcServer;
};

#endif