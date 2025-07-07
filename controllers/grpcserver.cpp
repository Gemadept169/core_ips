#include "grpcserver.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "entities/grpc_callback/sotcallback.h"
#include "utilities/logger.h"

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
        delete out;
        out = nullptr;
    }

    bool isOk = true;
    QString ipv4;
    unsigned int port;
    QJsonObject hostJson = json.value("host").toObject();
    if (const QJsonValue v = hostJson["ipv4"]; v.isString())
        ipv4 = v.toString();
    else
        isOk = false;
    if (const QJsonValue v = hostJson["port"]; v.isDouble())
        port = static_cast<unsigned int>(v.toInt());
    else
        isOk = false;

    unsigned int writerTimeoutMsecs;
    unsigned int trackLostFrameMax;
    QJsonObject sotCbJson = json.value("sotCallback").toObject();
    if (const QJsonValue v = sotCbJson["writerTimeoutMsecs"]; v.isDouble())
        writerTimeoutMsecs = static_cast<unsigned int>(v.toInt());
    else
        isOk = false;
    if (const QJsonValue v = sotCbJson["trackLostFrameMax"]; v.isDouble())
        trackLostFrameMax = static_cast<unsigned int>(v.toInt());
    else
        isOk = false;

    out = new GrpcServer(ipv4.toStdString(), port, parent);
    out->setSotCbWriterTimeoutMsecs(writerTimeoutMsecs);
    out->setSotCbTrackLostFrameMax(trackLostFrameMax);
    return isOk;
}

void GrpcServer::setSotCbTrackLostFrameMax(const unsigned int& frameMax) {
    if (_sotCallback) {
        _sotCallback->setTrackLostFrameMax(frameMax);
    }
}

void GrpcServer::setSotCbWriterTimeoutMsecs(const unsigned int& msec) {
    if (_sotCallback) {
        _sotCallback->setWriterTimeoutMsecs(msec);
    }
}

void GrpcServer::atStarted() {
    startServer();
}

void GrpcServer::atSotResults(const sot::SotInfo& info) {
    if (_sotCallback) {
        _sotCallback->pushResultData(info);
    }
}

void GrpcServer::atVideoIsConnected(const bool& isConnected) {
    _sotCallback->setIsVideoConnected(isConnected);
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
        LOG_INFO(QString("Grpc server listening on %1").arg(QString::fromStdString(server_address)));
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