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
}

GrpcServer::~GrpcServer() {
}

bool GrpcServer::fromJson(const QJsonObject& json, GrpcServer*& out, QObject* parent) {
    if (out) {
        return false;
    }
    out = new GrpcServer("localhost", 52124, parent);
    return true;
}

void GrpcServer::atStarted() {
    startServer();
}

void GrpcServer::atSotResults(const sot::SotInfo& info) {
    if (_sotCallback) {
        _sotCallback->pushResultData(info);
    }
}

void GrpcServer::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void GrpcServer::setPort(const unsigned int& port) {
    _port = port;
}

void GrpcServer::startServer() {
    QThread* grpcThread = QThread::create([this]() -> void {
        std::string server_address = this->_hostname + std::string(":") + std::to_string(this->_port);
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