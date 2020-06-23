#include "greenhouse/ds18b20.hpp"

#include "global_singletons.hpp"

namespace {

DallasTemperature dt;

}

namespace ns_ds18b20 {

uint32_t lastRequest = 0;

static constexpr uint8_t MINIMAL_RESOLUTION = 9;
static uint16_t CONVERSION_TIME;
static constexpr uint32_t TEMPERATURE_MEASURE_INTERVAL = 0.5 * 1000LU;

void init() {
  dt.setOneWire(getOneWire());
  dt.begin();
  dt.setResolution(MINIMAL_RESOLUTION);
  dt.setWaitForConversion(false);
  CONVERSION_TIME = dt.millisToWaitForConversion(MINIMAL_RESOLUTION);
  // CONVERSION_TIME = 1000;
}

bool refreshTemperatures() {
  if (millis() - lastRequest > TEMPERATURE_MEASURE_INTERVAL * 4) {
    dt.requestTemperatures();
    lastRequest = millis();
    return true;
  }
  return false;
}

}  // namespace ns_ds18b20

DS18B20::DS18B20(const DeviceAddress& address) : lastUpdate_(0) {
  memcpy(address_, address, 8);
}

int8_t DS18B20::getTemperature() {
  if (millis() > ns_ds18b20::lastRequest + ns_ds18b20::CONVERSION_TIME &&
      millis() - lastUpdate_ > ns_ds18b20::TEMPERATURE_MEASURE_INTERVAL) {
    cachedTemperature = dt.getTempC(address_);
    lastUpdate_ = millis();
  }
  return cachedTemperature;
}
