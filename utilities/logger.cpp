#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QMetaObject>
#include <QTextStream>
#include <QThread>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

Logger::Logger() : _level(Type::INFO),
                   _sink(Sink::CONSOLE_FILE),
                   _logDirPath("../logs/"),
                   _curLogFilePath(_logDirPath + "log.txt") {
    const std::string logDirStdPath = _logDirPath.toStdString();
    if (!fs::exists(logDirStdPath)) {
        fs::create_directory(logDirStdPath);
    }
}

void Logger::logToConsole(const Type& type, const QString& msg) {
    QTextStream out(stdout, QIODevice::WriteOnly);
    out << Logger::prependTypeInfo(type, msg) << Qt::endl;
}

void Logger::logToFile(const Type& type, const QString& msg) {
    QFile logFile(_curLogFilePath);
    if (logFile.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream out(&logFile);
        out << Logger::prependTypeInfo(type, msg) << Qt::endl;
    }
}

void Logger::prependExtraInfo(QString& extMsg, const QString& msg) {
    QTextStream ts(&extMsg);
    ts << "[" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz dd-MM-yyyy") + "] ["
       << QString::number((qulonglong)QThread::currentThreadId(), 16)
       << "] " << msg;
}

QString Logger::prependTypeInfo(const Type& type, const QString& msg) {
    QString typeString;
    switch (type) {
        case Type::ERROR:
            typeString = "[E] ";
            break;
        case Type::WARN:
            typeString = "[W] ";
            break;
        case Type::INFO:
            typeString = "[I] ";
            break;
        case Type::DEBUG:
            typeString = "[D] ";
            break;
        case Type::TRACE:
            typeString = "[T] ";
            break;
        default:
            break;
    }
    return typeString + msg;
}

Logger& Logger::instance() {
    static Logger _instance;
    return _instance;
}

void Logger::info(const QString& msg) {
    QString extMsg;
    Logger::prependExtraInfo(extMsg, msg);
    QMetaObject::invokeMethod(this, &Logger::log, Qt::QueuedConnection,
                              Logger::Type::INFO,
                              extMsg);
}
void Logger::warn(const QString& msg) {
    QString extMsg;
    Logger::prependExtraInfo(extMsg, msg);
    QMetaObject::invokeMethod(this, &Logger::log, Qt::QueuedConnection,
                              Logger::Type::WARN,
                              extMsg);
}
void Logger::error(const QString& msg) {
    QString extMsg;
    Logger::prependExtraInfo(extMsg, msg);
    QMetaObject::invokeMethod(this, &Logger::log, Qt::QueuedConnection,
                              Logger::Type::ERROR,
                              extMsg);
}
void Logger::debug(const QString& msg) {
    QString extMsg;
    Logger::prependExtraInfo(extMsg, msg);
    QMetaObject::invokeMethod(this, &Logger::log, Qt::QueuedConnection,
                              Logger::Type::DEBUG,
                              extMsg);
}
void Logger::trace(const QString& msg) {
    QString extMsg;
    Logger::prependExtraInfo(extMsg, msg);
    QMetaObject::invokeMethod(this, &Logger::log, Qt::QueuedConnection,
                              Logger::Type::TRACE,
                              extMsg);
}

void Logger::setSink(const Sink& sink) {
    _sink = sink;
}

void Logger::setLevel(const Type& type) {
    _level = type;
}

void Logger::log(const Type& type, const QString& msg) {
    switch (_sink) {
        case Sink::CONSOLE:
            logToConsole(type, msg);
            break;
        case Sink::FILE:
            logToFile(type, msg);
            break;
        case Sink::CONSOLE_FILE:
            logToConsole(type, msg);
            logToFile(type, msg);
            break;
        default:
            break;
    }
}