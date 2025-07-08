#ifndef ENTITIES_SYSTEM_INFO_TEMPERATURE_H
#define ENTITIES_SYSTEM_INFO_TEMPERATURE_H

#include <map>
#include <ostream>
#include <string>

class Temperature {
   public:
    explicit Temperature();
    Temperature(const Temperature &) = delete;
    Temperature &operator=(const Temperature &) = delete;
    ~Temperature();

    void refresh();
    friend std::ostream &operator<<(std::ostream &out, const Temperature &temp);

   private:
    std::map<std::string, float> _data;
};

#endif