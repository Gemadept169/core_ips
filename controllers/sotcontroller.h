#ifndef CONTROLLER_SOTCONTROLLER_H
#define CONTROLLER_SOTCONTROLLER_H

#include <QThread>

#include "entities/inner/sothandler.h"

class SotController final : public QObject {
    Q_OBJECT
   public:
    explicit SotController(QObject *parent = nullptr);
    SotController(const SotController &) = delete;
    SotController &operator=(const SotController &) = delete;
    ~SotController();

   signals:
    void startSot(const sot::BBox &initBox);
    void processSot(const cv::Mat &incomingFrame);
    void stopSot();
    void hasResult(const sot::SotInfo &result);

   public slots:
    void atStarted();

   private:
    SotHandler *_handler;
    QThread _handlerThread;
};

#endif