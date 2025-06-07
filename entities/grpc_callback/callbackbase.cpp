#include "callbackbase.h"

CallbackBase::CallbackBase(GrpcServer* grpcServer)
    : _grpcServer(grpcServer) {
}

CallbackBase::~CallbackBase() {
}

QPointer<GrpcServer> CallbackBase::grpcServer() const {
    return this->_grpcServer;
}
