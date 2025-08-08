#ifndef ENTITIES_VIDEO_CAPTURE_CVVIDEOCAPTURE_H
#define ENTITIES_VIDEO_CAPTURE_CVVIDEOCAPTURE_H

#include <QObject>
#include <QTimer>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class CvVideoCapture : public QObject {
    Q_OBJECT
   public:
    explicit CvVideoCapture(const QString &rtspPath, const uint &fps = 30, QObject *parent = nullptr);
    CvVideoCapture(const CvVideoCapture &) = delete;
    CvVideoCapture &operator=(const CvVideoCapture &) = delete;
    ~CvVideoCapture();

   signals:
    void hasVideoNewFrame(const cv::Mat &frame, const qint64 &createdAtMsecsSinceEpoch);
    void hasVideoDisconnected();

   public slots:
    void startCapture();
    void stopCapture();

   private slots:
    void readFrame();

   private:
    cv::Ptr<cv::VideoCapture> _cap;
    uint _fps;
    float _realFps;
    QString _rtspPath;
    QTimer *_readTimer;
};

#endif