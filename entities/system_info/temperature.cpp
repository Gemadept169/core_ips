#include "temperature.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Temperature::Temperature() {
}

Temperature::~Temperature() {
}

void Temperature::refresh() {
    for (const auto& dir : fs::directory_iterator("/sys/class/thermal/")) {
        if (dir.path().filename().string().find("thermal_zone") != std::string::npos) {
            const std::string labelPath = dir.path().string() + "/type";
            std::string tempPath = dir.path().string() + "/temp";
            std::ifstream labelFile(labelPath), tempFile(tempPath);
            std::string label, temp;
            std::getline(labelFile, label);
            std::getline(tempFile, temp);
            _data[label] = std::stof(temp) / 1000.0f;
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Temperature& temp) {
    for (const auto& pair : temp._data) {
        out << pair.first << "=" << pair.second << "°C ";
    }
    return out;
}
