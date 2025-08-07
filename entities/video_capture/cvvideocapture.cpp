#include "cvvideocapture.h"

#include "utilities/elapser.h"
#include "utilities/logger.h"

CvVideoCapture::CvVideoCapture(const QString &rtspPath, const uint &fps, QObject *parent)
    : QObject(parent),
      _fps(fps),
      _realFps(0.0f),
      _rtspPath(rtspPath),
      _cap(new cv::VideoCapture()),
      _readTimer(new QTimer(this)) {
    _readTimer->setInterval(static_cast<int>(1000 / _fps));
    QObject::connect(_readTimer, &QTimer::timeout, this, &CvVideoCapture::readFrame);
}

CvVideoCapture::~CvVideoCapture() {
}

void CvVideoCapture::startCapture() {
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
        LOG_INFO("Opened rtsp stream and start capturing")
    } else {
        LOG_WARN("Opening rtsp stream failed!")
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
    MEASURE_ELAPSED_FUNC(readMsec, {
        cv::Mat incomingFrame;
        if (!_cap.empty() && _cap->read(incomingFrame)) {
            emit hasVideoNewFrame(incomingFrame);
        } else {
            stopCapture();
            LOG_WARN("Can't read new frame")
            emit hasVideoDisconnected();
        }
    })
    if (readMsec > (1000.0f / (_fps + 20))) {
        _realFps = _realFps * 0.9f + (1000.0f / (readMsec)) * 0.1f;
        LOG_TRACE(QString("Reading frames %1 FPS").arg(_realFps))
    }
}
