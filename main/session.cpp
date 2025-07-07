#include "session.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>

#include "utilities/logger.h"

Session::Session(QObject* parent) : QObject(parent),
                                    _videoReader(nullptr),
                                    _grpcServer(nullptr),
                                    _sotController(new SotController()),
                                    _systemMonitor(new SystemMonitor()) {
    registerQMetaTypes();
    loadSession();

    _videoReader->moveToThread(&_videoThread);
    _sotController->moveToThread(&_sotThread);
    _grpcServer->moveToThread(&_grpcThread);
    _systemMonitor->moveToThread(&_systemThread);
    LOG_INSTACE.moveToThread(&_systemThread);

    initObjectConnections();
    startThreads();
    LOG_INFO("Wellcome new session!");
}

Session::~Session() {
    quitThreads();
}

void Session::handleSotTrackNewRequest(const sot::BBox& initBox) {
    if (!_videoReader || !_sotController || !_grpcServer) {
        return;
    }
    QMetaObject::invokeMethod(_sotController,
                              &SotController::startSot,
                              Qt::QueuedConnection,
                              initBox);
    QObject::connect(_videoReader, &VideoReader::hasVideoNewFrame, _sotController, &SotController::processSot);
    QObject::connect(_sotController, &SotController::hasResult, _grpcServer, &GrpcServer::atSotResults);
}

void Session::handleSotTrackStopRequest() {
    if (!_videoReader || !_sotController || !_grpcServer) {
        return;
    }
    QMetaObject::invokeMethod(_sotController,
                              &SotController::stopSot,
                              Qt::QueuedConnection);
    QObject::disconnect(_videoReader, &VideoReader::hasVideoNewFrame, _sotController, &SotController::processSot);
    QObject::disconnect(_sotController, &SotController::hasResult, _grpcServer, &GrpcServer::atSotResults);
}

void Session::registerQMetaTypes() {
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<sot::BBox>("sot::BBox");
    qRegisterMetaType<sot::SotInfo>("sot::SotInfo");
}

void Session::initObjectConnections() {
    QObject::connect(&_grpcThread, &QThread::started, _grpcServer, &GrpcServer::atStarted);
    QObject::connect(&_grpcThread, &QThread::finished, _grpcServer, &GrpcServer::deleteLater);
    QObject::connect(_grpcServer, &GrpcServer::hasSotTrackNewRequest, this, &Session::handleSotTrackNewRequest);
    QObject::connect(_grpcServer, &GrpcServer::hasSotTrackStop, this, &Session::handleSotTrackStopRequest);

    QObject::connect(&_videoThread, &QThread::started, _videoReader, &VideoReader::atStarted);
    QObject::connect(&_videoThread, &QThread::finished, _videoReader, &VideoReader::deleteLater);
    QObject::connect(_videoReader, &VideoReader::hasVideoIsConnected, _grpcServer, &GrpcServer::atVideoIsConnected);
    QObject::connect(_videoReader, &VideoReader::hasVideoIsConnected, [this](const bool& isConnected) -> void {
        if (!isConnected) {
            handleSotTrackStopRequest();
        }
    });

    QObject::connect(&_sotThread, &QThread::started, _sotController, &SotController::atStarted);
    QObject::connect(&_sotThread, &QThread::finished, _sotController, &SotController::deleteLater);

    QObject::connect(&_systemThread, &QThread::finished, _systemMonitor, &SotController::deleteLater);
}

void Session::loadSession() {
    QFile videoCfgFile("../configs/videoreader.json");
    if (!videoCfgFile.open(QIODevice::ReadOnly)) {
        qFatal() << "[Session] Couldn't open videoreader.json";
    }
    const auto videoCfgDoc(QJsonDocument::fromJson(videoCfgFile.readAll()));
    if (!VideoReader::fromJson(videoCfgDoc.object(), _videoReader)) {
        qFatal() << "[Session] Load config for VideoReader failed";
    };

    QFile grpcCfgFile("../configs/grpcserver.json");
    if (!grpcCfgFile.open(QIODevice::ReadOnly)) {
        qFatal() << "[Session] Couldn't open grpcserver.json";
    }
    const auto grpcCfgDoc(QJsonDocument::fromJson(grpcCfgFile.readAll()));
    if (!GrpcServer::fromJson(grpcCfgDoc.object(), _grpcServer)) {
        qFatal() << "[Session] Load config for GrpcServer failed";
    };
}

void Session::startThreads() {
    _grpcThread.start();
    _videoThread.start();
    _sotThread.start();
    _systemThread.start();
}

void Session::quitThreads() {
    _grpcThread.quit();
    _grpcThread.wait();

    _videoThread.quit();
    _videoThread.wait();

    _sotThread.quit();
    _sotThread.wait();

    _systemThread.quit();
    _systemThread.wait();
}