#include "jsonparser.h"

#include <QCoreApplication>

JsonParser::JsonParser() {
    loadForRead(NETWORK_CONFIG_PATH);
}

JsonParser::~JsonParser() {
}

JsonParser* JsonParser::getInstance() {
    static JsonParser* instance = nullptr;
    if (instance == nullptr) {
        instance = new JsonParser();
    }
    return instance;
}

void JsonParser::loadForRead(const QString& filename) {
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();
    QJsonDocument jsonValue = QJsonDocument::fromJson(value.toUtf8());
    _curJsonObject = jsonValue.object();
}

void JsonParser::setConfig(const QString& filename) {
    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath() + filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QJsonDocument jsonValue;
    jsonValue.setObject(_curJsonObject);
    file.write(jsonValue.toJson(QJsonDocument::Indented));
    file.close();
}

QJsonValue JsonParser::lookUpValue(const QString& key, const QJsonObject& jsonObj) {
    if (jsonObj.isEmpty()) {
        return _curJsonObject.value(key);
    }

    return jsonObj.value(key);
}

QJsonObject JsonParser::convertJsonValueToObject(const QJsonValue& value) {
    return value.toObject();
}

void JsonParser::updateValue(const QString& key, const QJsonObject& value) {
    _curJsonObject[key] = value;
}

void JsonParser::updateValue(const QString& key, const QJsonArray& value) {
    _curJsonObject[key] = value;
}

template <class T>
void JsonParser::updateValue(const QString& key, const T& value) {
    _curJsonObject[key] = value;
}
