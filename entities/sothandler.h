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
    void hasTrackResult(const sot::SotInfo &);

   public slots:
    void started();
    void startTracking(const ::sot::BBox &initBBox);
    void processTracking(const cv::Mat &frame);
    void stopTracking();

   private:
    std::unique_ptr<::sot::SotInferInterface> m_inferTrack;
    ::sot::BBox m_initBBoxTrack;
    bool m_isFirstTrack;
};
#endif