#include "sothandler.h"

SotHandler::SotHandler(QObject *parent)
    : QObject(parent),
      _engine(nullptr),
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

void SotHandler::atProcessTracking(const cv::Mat &frame) {
    if (_isFirstTrack) {
        _engine->init(frame, _initTrackBox);
        _isFirstTrack = false;
    } else {
        sot::SotInfo advSotInfo;
        _engine->update(frame, advSotInfo);
        emit hasResult(advSotInfo);
    }
}

void SotHandler::atStopTracking() {
    _isFirstTrack = false;
}