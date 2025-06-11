#include "videoreader.h"

VideoReader::VideoReader(QObject *parent)
    : QObject(parent),
      _checker(nullptr),
      _worker(nullptr) {
}

VideoReader::~VideoReader() {
}

bool VideoReader::fromJson(const QJsonObject &json, VideoReader *&out, QObject *parent) {
    if (out) {
        return false;
    }
    out = new VideoReader(parent);
    out->_checker = new VideoChecker("rtsp://192.168.1.250:8555/cam", 1000, out);  // TODO: read config from QJsonObject
    out->_worker = new CvVideoCapture("rtsp://192.168.1.250:8555/cam", 30, out);   // TODO: read config from QJsonObject
    out->initConnections();
    return true;
}

void VideoReader::atStarted() {
    qDebug() << "[VideoReader::atStarted]";
    _worker->startCapture();
}

void VideoReader::initConnections() {
    if (_checker && _worker) {
        QObject::connect(_worker, &CvVideoCapture::hasVideoDisconnected, _checker, &VideoChecker::atVideoDisconnected);
        QObject::connect(_worker, &CvVideoCapture::hasVideoDisconnected,
                         [this]() -> void {
                             emit this->hasVideoIsConnected(false);
                         });
        QObject::connect(_checker, &VideoChecker::hasVideoConnected, _worker, &CvVideoCapture::startCapture);
        QObject::connect(_checker, &VideoChecker::hasVideoConnected,
                         [this]() -> void {
                             emit this->hasVideoIsConnected(true);
                         });
        QObject::connect(_worker, &CvVideoCapture::hasVideoNewFrame, this, &VideoReader::hasVideoNewFrame);
    }
}
