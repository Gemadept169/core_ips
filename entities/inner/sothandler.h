#ifndef ENTITIES_SOTHANDLER_H
#define ENTITIES_SOTHANDLER_H

#include <QDebug>
#include <QMetaObject>
#include <QObject>

#include "sot/interfaces.hpp"
#include "sot/sot.hpp"
#include "sot/types.hpp"

class SotHandler : public QObject {
    Q_OBJECT
   public:
    explicit SotHandler(QObject *parent = nullptr);
    SotHandler(const SotHandler &) = delete;
    SotHandler &operator=(const SotHandler &) = delete;
    ~SotHandler();

   signals:
    void hasResult(const sot::SotInfo &);

   public slots:
    void atStarted();
    void atStartTracking(const ::sot::BBox &initBBox);
    void atProcessTracking(const cv::Mat &frame);
    void atStopTracking();

   private:
    std::unique_ptr<sot::SotInferInterface> _engine;
    sot::BBox _initTrackBox;
    bool _isFirstTrack;
};
#endif