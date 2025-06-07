#ifndef CONTROLLER_GRPCSERVER_H
#define CONTROLLER_GRPCSERVER_H

#include <grpc/grpc.h>
#include <grpcpp/server.h>

#include <QObject>
#include <QPointer>
#include <QThread>
#include <memory>

#include "sot/types.hpp"

class SotCallback;
class GrpcServer : public QObject {
    Q_OBJECT
   public:
    GrpcServer(const std::string &hostname, const unsigned int &port, QObject *parent = nullptr);
    GrpcServer(const GrpcServer &) = delete;
    GrpcServer &operator=(const GrpcServer &) = delete;
    ~GrpcServer();

   signals:
    void hasSotTrackNewRequest(const sot::BBox &initBox);
    void hasSotTrackStop();

   public slots:
    void atStarted();
    void atSotResults(const sot::SotInfo &info);

   public:
    void setHostname(const std::string &hostname);
    void setPort(const unsigned int &port);
    void startServer();
    void stopServer();

   private:
    void registerQtMetaTypes();

    SotCallback *_sotCallback;
    std::string _hostname;
    unsigned int _port;
    std::unique_ptr<grpc::Server> _server;
};

#endif