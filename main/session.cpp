#include "session.h"

Session::Session(QObject* parent) : QObject(parent),
                                    _videoReader(nullptr),
                                    _sotController(new SotController()),
                                    _grpcServer(new GrpcServer("localhost", 1234)) {
    registerQMetaTypes();

    _grpcServer->moveToThread(&_grpcThread);
    _sotController->moveToThread(&_sotThread);
    VideoReader::fromJson(QJsonObject(), _videoReader);
    _videoReader->moveToThread(&_videoThread);
    initObjectConnections();
    startThreads();
}

Session::~Session() {
    quitThreads();
}

void Session::registerQMetaTypes() {
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<sot::BBox>("sot::BBox");
    qRegisterMetaType<sot::SotInfo>("sot::SotInfo");
}

void Session::initObjectConnections() {
    QObject::connect(&_grpcThread, &QThread::started, _grpcServer, &GrpcServer::atStarted);
    QObject::connect(&_grpcThread, &QThread::finished, _grpcServer, &GrpcServer::deleteLater);

    QObject::connect(&_videoThread, &QThread::started, _videoReader, &VideoReader::atStarted);
    QObject::connect(&_videoThread, &QThread::finished, _videoReader, &VideoReader::deleteLater);

    QObject::connect(&_sotThread, &QThread::started, _sotController, &SotController::atStarted);
    QObject::connect(&_sotThread, &QThread::finished, _sotController, &SotController::deleteLater);
    // QObject::connect(&_grpcThread, &GrpcServer::hasNewSotTrackRequest, /* trick new for image processing control */);
    // QObject::connect(&_grpcThread, &GrpcServer::hasSotTrackStop, /* trick new for image processing control */);
}

void Session::startThreads() {
    _grpcThread.start();
    _videoThread.start();
    _sotThread.start();
}

void Session::quitThreads() {
    _grpcThread.quit();
    _grpcThread.wait();

    _videoThread.quit();
    _videoThread.wait();

    _sotThread.quit();
    _sotThread.wait();
}

void Session::initTimers() {
}
