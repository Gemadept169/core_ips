#include "systemmonitor.h"

#include <sstream>
#include <string>

#include "utilities/logger.h"

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject(parent),
      _refreshTimer(new QTimer(this)) {
    _refreshTimer->setInterval(5000);
    QObject::connect(_refreshTimer, &QTimer::timeout, this, &SystemMonitor::refresh);
    _refreshTimer->start();
}

void SystemMonitor::refresh() {
    _temperature.refresh();
    std::ostringstream tempOss;
    tempOss << "Temp: " << _temperature;
    LOG_INFO(QString::fromStdString(tempOss.str()));

    _power.refresh();
    std::ostringstream powerOss;
    powerOss << "Power: " << _power;
    LOG_INFO(QString::fromStdString(powerOss.str()));
}

SystemMonitor::~SystemMonitor() {
}