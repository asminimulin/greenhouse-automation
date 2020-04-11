#ifndef DS18B20_HPP
#define DS18B20_HPP

#include <one_wire_driver.hpp>


#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>


namespace ns_ds18b20 {

void init();


bool refreshTemperatures();


}


class DS18B20 {
public:
    DS18B20(const DeviceAddress& address);
    int8_t getTemperature();

private:
    int8_t cachedTemperature;
    DeviceAddress address_;
};

#endif