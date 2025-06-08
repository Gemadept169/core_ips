#include <QCoreApplication>
#include <QDebug>
#include <QPointer>
#include <QSettings>
#include <QString>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "session.h"

#ifndef CORE_IPS_STRING_VERSION
#define CORE_IPS_STRING_VERSION "unknown"
#endif

int main(int argc, char* argv[]) {
    if (argc > 1 &&
        (std::string(argv[1]) == "--version" ||
         std::string(argv[1]) == "-v")) {
        std::cout << CORE_IPS_STRING_VERSION << std::endl;
        return 0;
    }
    // cv::Mat tmpMat = cv::Mat::ones(1024, 1024, CV_8UC3);
    // tmpMat.at<int>(0, 0) = 255;
    // tmpMat.at<int>(0, 1) = 255;
    // tmpMat.at<int>(0, 2) = 255;
    // cv::imwrite("some.jpg", tmpMat);
    // std::cout << "tmpMat: " << tmpMat << std::endl;
    // cv::imshow("Output", tmpMat);
    // cv::waitKey(0);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(CORE_IPS_STRING_VERSION);
    Session s{&app};
    // QJsonObject core_ips = JsonParser::getInstance()->convertJsonValueToObject(JsonParser::getInstance()->lookUpValue(QString("core_ips")));
    // QString q1 = JsonParser::getInstance()->lookUpValue(QString("address"), core_ips).toString();
    // quint32 q2 = JsonParser::getInstance()->lookUpValue(QString("port"), core_ips).toInt();
    // qDebug() << "+== " << q1 << q2;
    // qDebug() << "+== [Main]" << QThread::currentThread();
    // Session session(&app);
    return app.exec();
};
