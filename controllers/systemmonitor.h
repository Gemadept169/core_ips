#ifndef CONTROLLER_SYSTEMMONITOR_H
#define CONTROLLER_SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>

#include "entities/system_info/power.h"
#include "entities/system_info/temperature.h"

class SystemMonitor final : public QObject {
    Q_OBJECT
   public:
    explicit SystemMonitor(QObject *parent = nullptr);
    SystemMonitor(const SystemMonitor &) = delete;
    SystemMonitor &operator=(const SystemMonitor &) = delete;
    ~SystemMonitor();

    // static bool fromJson(const QJsonObject &json, SystemMonitor *&out, QObject *parent = nullptr);

   private slots:
    void refresh();

   private:
    void initConnections();

    QTimer *_refreshTimer;
    Temperature _temperature;
    Power _power;
};

#endif