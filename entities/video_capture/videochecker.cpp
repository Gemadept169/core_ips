#include "videochecker.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QString>

#include "utilities/logger.h"

namespace {
bool checkRtspServerIsAvailable(const QString &rtspPath) {
    bool isOk = false;
    QProcess ffprobe;
    QObject::connect(&ffprobe, &QProcess::readyReadStandardOutput,
                     [&isOk, &ffprobe]() -> void {
                         const QString out(ffprobe.readAllStandardOutput().simplified());
                         if (out.contains("\"width\": 1920") &&
                             out.contains("\"height\": 1080") &&
                             out.contains("\"codec_type\": \"video\"") &&
                             out.contains("\"format_name\": \"rtsp\"")) {
                             isOk = true;
                         }
                     });
    ffprobe.start("ffprobe", QStringList() << "-v" << "error"
                                           << "-show_streams"
                                           << "-show_format"
                                           << "-of" << "json"
                                           << "-i" << rtspPath);
    ffprobe.waitForFinished(5000);
    ffprobe.close();
    return isOk;
};
}  // namespace

VideoChecker::VideoChecker(const QString &rtspPath, const uint &retryIntervalMsec, QObject *parent)
    : QObject(parent),
      _retryTimer(new QTimer(this)),
      _rtspPath(rtspPath),
      _retryIntervalMsec(retryIntervalMsec) {
    _retryTimer->setInterval(_retryIntervalMsec);
    QObject::connect(_retryTimer, &QTimer::timeout, this, &VideoChecker::checkConnection);
}

VideoChecker::~VideoChecker() {
}

void VideoChecker::atVideoDisconnected() {
    LOG_WARN("The video streaming disconnected!")
    if (_retryTimer && !_retryTimer->isActive()) {
        _retryTimer->start();
    }
}

void VideoChecker::checkConnection() {
    LOG_INFO("Checking video streaming connection...")
    if (checkRtspServerIsAvailable(_rtspPath)) {
        LOG_INFO("Rtsp server is ready")
        emit hasVideoConnected();
        if (_retryTimer && _retryTimer->isActive()) {
            _retryTimer->stop();
        }
    }
}
