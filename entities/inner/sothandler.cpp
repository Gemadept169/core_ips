#include "sothandler.h"

#include <QDateTime>

#include "utilities/elapser.h"
#include "utilities/logger.h"

SotHandler::SotHandler(QObject *parent)
    : QObject(parent),
      _engine(nullptr),
      _processFps(0.0f),
      _isFirstTrack(false) {
}

SotHandler::~SotHandler() {
}

void SotHandler::atStarted() {
    if (_engine) {
        _engine.reset();
    }
    sot::SotInferInterface *inferPtr{nullptr};
    sot::SiamrpnFactory siamprnFac = sot::SiamrpnFactory();
    siamprnFac.orderSotEngine(inferPtr);
    _engine = std::unique_ptr<sot::SotInferInterface>(inferPtr);
}

void SotHandler::atStartTracking(const sot::BBox &initBBox) {
    _isFirstTrack = true;
    _initTrackBox = initBBox;
}

void SotHandler::atProcessTracking(const cv::Mat &frame, const qint64 &frameCreatedAtMsecsSinceEpoch) {
    LOG_TRACE(QString(
                  "Sot delay reader -> atProcessTracking: %1 ms")
                  .arg(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - frameCreatedAtMsecsSinceEpoch))

    sot::SotInfo advSotInfo;
    MEASURE_ELAPSED_FUNC(processMsec, {
        if (_isFirstTrack) {
            _engine->init(frame, _initTrackBox);
            _isFirstTrack = false;
        } else {
            _engine->update(frame, advSotInfo);
            emit hasResult(advSotInfo, frameCreatedAtMsecsSinceEpoch);
        }
    })
    if (processMsec > 0.5f) {
        _processFps = _processFps * 0.9f + (1000.0f / processMsec) * 0.1f;
        LOG_TRACE(QString("Sot info w, h: %1, %2").arg(advSotInfo.bbox.width).arg(advSotInfo.bbox.height))
        LOG_TRACE(QString("Sot processing frames %1 FPS").arg(QString::number(_processFps)))
    }
}

void SotHandler::atStopTracking() {
    _isFirstTrack = false;
}