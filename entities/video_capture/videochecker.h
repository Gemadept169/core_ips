#ifndef ENTITIES_VIDEO_CAPTURE_VIDEOCHECKER_H
#define ENTITIES_VIDEO_CAPTURE_VIDEOCHECKER_H

#include <QObject>
#include <QTimer>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class VideoChecker : public QObject {
    Q_OBJECT
   public:
    explicit VideoChecker(const QString &rtspPath, const uint &retryIntervalMsec = 1000, QObject *parent = nullptr);
    VideoChecker(const VideoChecker &) = delete;
    VideoChecker &operator=(const VideoChecker &) = delete;
    ~VideoChecker();

   signals:
    void hasVideoConnected();

   public slots:
    void atVideoDisconnected();

   private slots:
    void checkConnection();

   private:
    cv::Ptr<cv::VideoCapture> _cap;
    uint _retryIntervalMsec;
    QString _rtspPath;
    QTimer *_retryTimer;
};

#endif