#ifndef MAIN_SESSION_H
#define MAIN_SESSION_H

#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QThread>
#include <QTimer>

#include "controllers/grpcserver.h"

class Session : public QObject {
    Q_OBJECT
   public:
    explicit Session(QObject *parent = nullptr);
    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    ~Session();

   private:
    // void registerMetatype void initObjectConnections();
    void initObjectConnections();
    void startThreads();
    void quitThreads();
    void initTimers();

   private:
    QThread _grpcServerThread;
    GrpcServer _grpcServer;
};

#endif