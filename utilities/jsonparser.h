#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <memory>

class JsonParser {
   public:
    explicit JsonParser();
    JsonParser(const JsonParser &) = delete;
    JsonParser &operator=(const JsonParser &) = delete;
    ~JsonParser();

    static JsonParser *getInstance();

    void loadForRead(const QString &filename);

    void setConfig(const QString &filename);

    QJsonValue lookUpValue(const QString &key, const QJsonObject &jsonObj = QJsonObject());

    QJsonObject convertJsonValueToObject(const QJsonValue &value);

    template <typename T>
    void updateValue(const QString &key, const T &value);

    void updateValue(const QString &key, const QJsonObject &value);

    void updateValue(const QString &key, const QJsonArray &value);

   private:
    QString NETWORK_CONFIG_PATH{"/home/nvidia/Projects/core_ips/configs/network.json"};
    QJsonObject _curJsonObject;
};

#endif  // CONFIGPARSER_H
