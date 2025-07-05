#ifndef CONTROLLER_GRPCSERVER_H
#define CONTROLLER_GRPCSERVER_H

#include <grpcpp/server.h>

#include <QObject>
#include <memory>

#include "sot/types.hpp"

class SotCallback;
class GrpcServer final : public QObject {
    Q_OBJECT
   public:
    GrpcServer(const std::string &hostname, const unsigned int &port, QObject *parent = nullptr);
    GrpcServer(const GrpcServer &) = delete;
    GrpcServer &operator=(const GrpcServer &) = delete;
    ~GrpcServer();

    static bool fromJson(const QJsonObject &json, GrpcServer *&out, QObject *parent = nullptr);

    void setSotCbTrackLostFrameMax(const unsigned int &frameMax);

    void setSotCbWriterTimeoutMsecs(const unsigned int &msec);

   signals:
    void hasSotTrackNewRequest(const sot::BBox &initBox);
    void hasSotTrackStop();

   public slots:
    void atStarted();
    void atSotResults(const sot::SotInfo &info);
    void atVideoIsConnected(const bool &isConnected);

   private:
    void setHostname(const std::string &hostname);
    void setPort(const unsigned int &port);
    void startServer();
    void stopServer();

    SotCallback *_sotCallback;
    std::string _hostname;
    unsigned int _port;
    std::unique_ptr<grpc::Server> _server;
};

#endif