#ifndef CONTROLLER_VIDEOREADER_H
#define CONTROLLER_VIDEOREADER_H

#include <QJsonObject>
#include <QMetaObject>

#include "entities/video_capture/cvvideocapture.h"
#include "entities/video_capture/videochecker.h"

class VideoReader final : public QObject {
    Q_OBJECT
   public:
    explicit VideoReader(QObject *parent = nullptr);
    VideoReader(const VideoReader &) = delete;
    VideoReader &operator=(const VideoReader &) = delete;
    ~VideoReader();

    static bool fromJson(const QJsonObject &json, VideoReader *&out, QObject *parent = nullptr);

   signals:
    void hasVideoNewFrame(const cv::Mat &frame);
    void hasVideoIsConnected(const bool &isConnected);

   public slots:
    void atStarted();

   private:
    void initConnections();

    VideoChecker *_checker;
    CvVideoCapture *_worker;
};

#endif