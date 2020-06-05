#include "one_wire_driver/one_wire_driver.hpp"
#include "greenhouse/ds18b20.hpp"

namespace {

DallasTemperature dt;

}

namespace ns_ds18b20 {

static constexpr uint8_t MINIMAL_RESOLUTION = 9;
static uint16_t CONVERSION_TIME;


void init() {
    dt.setOneWire(getOneWire());
    dt.begin();
    dt.setResolution(MINIMAL_RESOLUTION);
    dt.setWaitForConversion(false);
    //CONVERSION_TIME = dt.millisToWaitForConversion(MINIMAL_RESOLUTION);
    CONVERSION_TIME = 1000;
}


bool refreshTemperatures() {
    static uint32_t lastRequest = 0;
    if (millis() - lastRequest > CONVERSION_TIME) {
        dt.requestTemperatures();
        return true;
    }
    return false;
}


} // namespace ns_ds18b20


DS18B20::DS18B20(const DeviceAddress& address) {
    memcpy(address_, address, 8);
}


int8_t DS18B20::getTemperature() {
    if (ns_ds18b20::refreshTemperatures()) {
        cachedTemperature = dt.getTempC(address_);
    }
    return cachedTemperature;
}
