#include "sothandler.h"

SotHandler::SotHandler(QObject *parent)
    : QObject(parent),
      m_inferTrack(nullptr),
      m_isFirstTrack(false) {
    qDebug() << "Hallo";
}

SotHandler::~SotHandler() {
}

void SotHandler::started() {
    if (this->m_inferTrack) {
        this->m_inferTrack.reset();
    }
    ::sot::SotInferInterface *inferPtr{nullptr};
    ::sot::SiamrpnFactory siamprnFac = ::sot::SiamrpnFactory();
    siamprnFac.orderSotEngine(inferPtr);
    this->m_inferTrack = std::unique_ptr<::sot::SotInferInterface>(inferPtr);
}

void SotHandler::startTracking(const sot::BBox &initBBox) {
    this->m_isFirstTrack = true;
    this->m_initBBoxTrack = initBBox;
}

void SotHandler::processTracking(const cv::Mat &frame) {
    if (this->m_isFirstTrack) {
        this->m_inferTrack->init(frame, this->m_initBBoxTrack);
        this->m_isFirstTrack = false;
        emit hasTrackResult(sot::SotInfo());
    } else {
        sot::SotInfo advSotInfo;
        this->m_inferTrack->update(frame, advSotInfo);
        emit hasTrackResult(advSotInfo);
    }
}

void SotHandler::stopTracking() {
    this->m_isFirstTrack = false;
}