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
        delete out;
        out = nullptr;
    }
    out = new VideoReader(parent);

    bool isOk = true;
    QString rtspSource;
    uint retryIntervalMsec;
    uint fps;

    if (const QJsonValue v = json["rtspSource"]; v.isString())
        rtspSource = v.toString();
    else
        isOk = false;
    if (const QJsonValue v = json["retryIntervalMsec"]; v.isDouble())
        retryIntervalMsec = static_cast<unsigned int>(v.toInt());
    else
        isOk = false;
    if (const QJsonValue v = json["videoFps"]; v.isDouble())
        fps = static_cast<unsigned int>(v.toInt());
    else
        isOk = false;

    out->_checker = new VideoChecker(rtspSource, retryIntervalMsec, out);
    out->_worker = new CvVideoCapture(rtspSource, fps, out);
    out->initConnections();
    return isOk;
}

void VideoReader::atStarted() {
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
