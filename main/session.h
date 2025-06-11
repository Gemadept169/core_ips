#ifndef MAIN_SESSION_H
#define MAIN_SESSION_H

#include <QMetaObject>
#include <QObject>
#include <QSharedPointer>
#include <QThread>
#include <QTimer>

#include "controllers/grpcserver.h"
#include "controllers/sotcontroller.h"
#include "controllers/videoreader.h"

class Session : public QObject {
    Q_OBJECT
   public:
    explicit Session(QObject *parent = nullptr);
    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    ~Session();

   private:
    // void registerMetatype void initObjectConnections();
    void registerQMetaTypes();
    void initObjectConnections();
    void startThreads();
    void quitThreads();
    void initTimers();

   private:
    QThread _grpcThread;
    QThread _videoThread;
    QThread _sotThread;

    GrpcServer *_grpcServer;
    VideoReader *_videoReader;
    SotController *_sotController;
};

#endif