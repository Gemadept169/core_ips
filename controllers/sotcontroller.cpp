#include "sotcontroller.h"

SotController::SotController(QObject *parent)
    : QObject(parent),
      _handler(nullptr) {
}

SotController::~SotController() {
    _handlerThread.quit();
    _handlerThread.wait();
}

void SotController::atStarted() {
    _handler = new SotHandler();
    _handler->moveToThread(&_handlerThread);

    connect(&_handlerThread, &QThread::started, _handler, &SotHandler::atStarted);
    connect(&_handlerThread, &QThread::finished, _handler, &SotHandler::deleteLater);

    connect(this, &SotController::startSot, _handler, &SotHandler::atStartTracking);
    connect(this, &SotController::processSot, _handler, &SotHandler::atProcessTracking);
    connect(this, &SotController::stopSot, _handler, &SotHandler::atStopTracking);
    connect(_handler, &SotHandler::hasResult, this, &SotController::hasResult);

    _handlerThread.start();
}