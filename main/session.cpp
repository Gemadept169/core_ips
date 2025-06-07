#include "session.h"

Session::Session(QObject* parent) : QObject(parent),
                                    _grpcServer(GrpcServer("localhost", 1234)) {
    initObjectConnections();
    _grpcServer.moveToThread(&_grpcServerThread);
    startThreads();
}

Session::~Session() {
    quitThreads();
}

void Session::initObjectConnections() {
    QObject::connect(&_grpcServerThread, &QThread::started, &_grpcServer, &GrpcServer::atStarted);
    QObject::connect(&_grpcServerThread, &QThread::finished, &_grpcServer, &GrpcServer::deleteLater);
    // QObject::connect(&_grpcServerThread, &GrpcServer::hasNewSotTrackRequest, /* trick new for image processing control */);
    // QObject::connect(&_grpcServerThread, &GrpcServer::hasSotTrackStop, /* trick new for image processing control */);
}

void Session::startThreads() {
    _grpcServerThread.start();
}

void Session::quitThreads() {
    _grpcServerThread.quit();
    _grpcServerThread.wait();
}

void Session::initTimers() {
}
