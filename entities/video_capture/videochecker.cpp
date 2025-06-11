#include "videochecker.h"

#include <QDebug>

VideoChecker::VideoChecker(const QString &rtspPath, const uint &retryIntervalMsec, QObject *parent)
    : QObject(parent),
      _cap(new cv::VideoCapture()),
      _retryTimer(new QTimer(this)),
      _rtspPath(rtspPath),
      _retryIntervalMsec(retryIntervalMsec) {
    _retryTimer->setInterval(_retryIntervalMsec);  // TODO: must be read from config file
    QObject::connect(_retryTimer, &QTimer::timeout, this, &VideoChecker::checkConnection);
}

VideoChecker::~VideoChecker() {
}

void VideoChecker::atVideoDisconnected() {
    qDebug() << "[VideoChecker::atVideoDisconnected]";
    if (_retryTimer && !_retryTimer->isActive()) {
        _retryTimer->start();
    }
}

void VideoChecker::checkConnection() {
    qDebug() << "[VideoChecker::checkConnection]";
    emit hasVideoConnected();
    if (_retryTimer && _retryTimer->isActive()) {
        _retryTimer->stop();
    }
}

// void VideoChecker::checkConnection() {
//     qDebug() << "[VideoChecker::checkConnection]";
//     if (_cap->open(QString("rtspsrc location=%1 "
//                            "! appsink")
//                        .arg(_rtspPath)
//                        .toUtf8()
//                        .constData(),
//                    cv::CAP_GSTREAMER)) {
//         qDebug() << "[VideoChecker::checkConnection] Video is connected";
//         emit hasVideoConnected();
//         if (_retryTimer && _retryTimer->isActive()) {
//             _retryTimer->stop();
//         }
//     } else {
//         _cap->release();
//     }
// }
