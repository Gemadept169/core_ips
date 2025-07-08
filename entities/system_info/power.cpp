#include "power.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Power::Power() : _railNums(3) {
}

Power::~Power() {
}

void Power::refresh() {
    // Inspect 3-channel INA3221 power monitors at I2C addresses 0x40
    for (const auto& dir : fs::directory_iterator("/sys/bus/i2c/drivers/ina3221/1-0040/hwmon/")) {
        if (dir.path().filename().string().find("hwmon") != std::string::npos) {
            for (uint railIdx = 1; railIdx < _railNums + 1; ++railIdx) {
                const std::string railIdxStr = std::to_string(railIdx);
                const std::string labelPath = dir.path().string() + "/in" + railIdxStr + "_label";
                const std::string voltPath = dir.path().string() + "/in" + railIdxStr + "_input";
                const std::string currPath = dir.path().string() + "/curr" + railIdxStr + "_input";
                std::ifstream labelFile(labelPath), voltFile(voltPath), currFile(currPath);
                std::string labelMilliWat, millivolts, milliamperes;
                std::getline(labelFile, labelMilliWat);
                std::getline(voltFile, millivolts);
                std::getline(currFile, milliamperes);
                _data[labelMilliWat] = (std::stof(millivolts) / 1000.0f) * std::stof(milliamperes);
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Power& power) {
    for (const auto& pair : power._data) {
        out << pair.first << "=" << pair.second << "mW ";
    }
    return out;
}
