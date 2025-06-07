#include "grpcserver.h"

#include <QDebug>

#include "entities/grpc_callback/sotcallback.h"

#define DEBUG_PREFIX_GRPCSERVER "[GrpcServer]"

GrpcServer::GrpcServer(const std::string& hostname,
                       const unsigned int& port,
                       QObject* parent)
    : QObject(parent),
      _hostname(hostname),
      _port(port),
      _sotCallback(new SotCallback(this)) {
    registerQtMetaTypes();
}

GrpcServer::~GrpcServer() {
}

void GrpcServer::atStarted() {
    startServer();
}

void GrpcServer::atSotResults(const sot::SotInfo& info) {
    // if (_ipsSotCallback) {
    //     _ipsSotCallback->pushResultData(info);
    // }
}

void GrpcServer::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void GrpcServer::setPort(const unsigned int& port) {
    _port = port;
}

void GrpcServer::startServer() {
    QThread* grpcThread = QThread::create([this]() -> void {
        std::string server_address{"192.168.0.106:52124"};  // TODO: Must be read from a config file
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(_sotCallback);
        _server = builder.BuildAndStart();
        qDebug() << DEBUG_PREFIX_GRPCSERVER << "Server listening on" << server_address;
        _server->Wait();
    });
    QObject::connect(grpcThread, &QThread::finished, grpcThread, &QThread::deleteLater);
    grpcThread->start();
}

void GrpcServer::stopServer() {
    if (_server) {
        _server->Shutdown();
    }
}

void GrpcServer::registerQtMetaTypes() {
    qRegisterMetaType<sot::BBox>("sot::BBox");
    qRegisterMetaType<sot::SotInfo>("sot::SotInfo");
}