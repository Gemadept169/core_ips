#include <QCoreApplication>

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

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(CORE_IPS_STRING_VERSION);
    Session session(&app);
    return app.exec();
};
