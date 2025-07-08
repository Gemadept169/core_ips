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

Logger::Logger() : _level(Type::TRACE),
                   _sink(Sink::CONSOLE_FILE),
                   _logDirPath("../logs/"),
                   _rotatingFilesNum(3),
                   _rotatingBytesMax4One(3 * 1024 * 1024) {
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
    QFile curLogFile(_logDirPath + "log.txt.1");
    if (curLogFile.size() >= _rotatingBytesMax4One) {
        // Remove the olders file
        std::string logDirPath = _logDirPath.toStdString() + "log.txt.";
        const std::string oldestPath = logDirPath + std::to_string(_rotatingFilesNum);
        if (fs::exists(oldestPath)) {
            fs::remove(oldestPath);
        }
        // Shifting number of file indies
        for (int lastIdx = _rotatingFilesNum; lastIdx > 1; lastIdx--) {
            const std::string fromFilePath = logDirPath + std::to_string(lastIdx - 1);
            const std::string toFilePath = logDirPath + std::to_string(lastIdx);
            if (fs::exists(fromFilePath) && !fs::exists(toFilePath)) {
                fs::rename(fromFilePath, toFilePath);
            }
        }
    }
    if (curLogFile.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream out(&curLogFile);
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
    if (type > _level) return;
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