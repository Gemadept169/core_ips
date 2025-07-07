#ifndef ENTITIES_SYSTEM_INFO_POWER_H
#define ENTITIES_SYSTEM_INFO_POWER_H

#include <map>
#include <ostream>
#include <string>

class Power {
   public:
    explicit Power();
    Power(const Power &) = delete;
    Power &operator=(const Power &) = delete;
    ~Power();
    void refresh();
    friend std::ostream &operator<<(std::ostream &out, const Power &temp);

   private:
    std::map<std::string, float> _data;
};

#endif