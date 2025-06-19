#include "cvvideocapture.h"

#include <QDebug>

CvVideoCapture::CvVideoCapture(const QString &rtspPath, const uint &fps, QObject *parent)
    : QObject(parent),
      _fps(fps),
      _rtspPath(rtspPath),
      _cap(new cv::VideoCapture()),
      _readTimer(new QTimer(this)) {
    _readTimer->setInterval(static_cast<int>(1000 / _fps));
    QObject::connect(_readTimer, &QTimer::timeout, this, &CvVideoCapture::readFrame);
}

CvVideoCapture::~CvVideoCapture() {
}

void CvVideoCapture::startCapture() {
    qDebug() << "[CvVideoCapture::startCapture]";
    if (_cap->isOpened()) {
        stopCapture();
    }
    // if (_cap->open(QString("rtspsrc location=%1 "
    //                        "! queue ! rtph264depay ! h264parse ! nvv4l2decoder ! nvvidconv ! appsink")
    //                    .arg(_rtspPath)
    //                    .toUtf8()
    //                    .constData(),
    //                cv::CAP_GSTREAMER)) {
    if (_cap->open(QString("rtspsrc location=%1 latency=100 tcp-timeout=1 "
                           "! queue ! rtph264depay ! h264parse ! avdec_h264 output-corrupt=false ! videoconvert ! appsink")
                       .arg(_rtspPath)
                       .toUtf8()
                       .constData(),
                   cv::CAP_GSTREAMER)) {
        if (_readTimer && !_readTimer->isActive()) {
            _readTimer->start();
        }
        qDebug() << "[CvVideoCapture::startCapture] Opened rtsp stream by Gstreamer";
    } else {
        qDebug() << "[CvVideoCapture::startCapture] Disconnected rtsp stream by Gstreamer";
        emit hasVideoDisconnected();
    }
}

void CvVideoCapture::stopCapture() {
    if (_readTimer && _readTimer->isActive()) {
        _readTimer->stop();
    }
    if (!_cap.empty() && _cap->isOpened()) {
        _cap->release();
    }
}

void CvVideoCapture::readFrame() {
    cv::Mat incomingFrame;
    if (!_cap.empty() && _cap->read(incomingFrame)) {
        emit hasVideoNewFrame(incomingFrame);
    } else {
        stopCapture();
        qDebug() << "[CvVideoCapture::readFrame] can't read frame";
        emit hasVideoDisconnected();
    }
}
