#include "PlatformServer.hpp"

#include <ctime>

#include "App.hpp"
#include "platform/time.hpp"

void PlatformServer::runMainLoop(App& app) {
    while (!app.wantToQuit()) {
        app.update();

        sleepMs(20);
    }
}

std::string PlatformServer::getDateString(int s) {
    time_t tm = s;

    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&tm));

    return std::string(mbstr);
}