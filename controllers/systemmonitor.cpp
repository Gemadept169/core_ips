#include "systemmonitor.h"

#include <iostream>

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject(parent),
      _refreshTimer(new QTimer(this)) {
    _refreshTimer->setInterval(1000);
    QObject::connect(_refreshTimer, &QTimer::timeout, this, &SystemMonitor::refresh);
    _refreshTimer->start();
}

void SystemMonitor::refresh() {
    _temperature.refresh();
    std::cout << _temperature << std::endl;
}

SystemMonitor::~SystemMonitor() {
}