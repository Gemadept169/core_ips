#ifndef OUTER_UTILITIES_LOGGER_H
#define OUTER_UTILITIES_LOGGER_H

#include <QObject>
#include <QString>

#define LOG_INSTACE Logger::instance()
#define LOG_INFO(msg) LOG_INSTACE.info(msg)
#define LOG_WARN(msg) LOG_INSTACE.warn(msg)
#define LOG_ERROR(msg) LOG_INSTACE.error(msg)
#define LOG_DEBUG(msg) LOG_INSTACE.debug(msg)
#define LOG_TRACE(msg) LOG_INSTACE.trace(msg)

class Logger : public QObject {
    Q_OBJECT
   public:
    static Logger& instance();
    enum class Sink : unsigned char {
        NONE = 0x0,
        CONSOLE = (1 << 0),
        FILE = (1 << 1),
        CONSOLE_FILE = (CONSOLE | FILE)
    };

    enum class Type : unsigned char {
        ERROR,
        WARN,
        INFO,
        DEBUG,
        TRACE
    };

    void info(const QString& msg);
    void warn(const QString& msg);
    void error(const QString& msg);
    void debug(const QString& msg);
    void trace(const QString& msg);

    void setSink(const Sink& sink);
    void setLevel(const Type& type);

   private slots:
    void log(const Type& type, const QString& msg);

   private:
    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static void prependExtraInfo(QString& extMsg, const QString& msg);
    static QString prependTypeInfo(const Type& type, const QString& msg);
    void logToConsole(const Type& type, const QString& msg);
    void logToFile(const Type& type, const QString& msg);

    QString _logDirPath;
    QString _curLogFilePath;
    Sink _sink;
    Type _level;
};

#endif