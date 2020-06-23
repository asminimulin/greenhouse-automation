#ifndef DS18B20_HPP
#define DS18B20_HPP

#include <global_singletons.hpp>

#define REQUIRESNEW false
#define REQUIRESALARMS false
#include <DallasTemperature.h>

namespace ns_ds18b20 {

void init();

bool refreshTemperatures();

}  // namespace ns_ds18b20

class DS18B20 {
 public:
  DS18B20(const DeviceAddress& address);
  virtual int8_t getTemperature();

 private:
  uint32_t lastUpdate_;
  int8_t cachedTemperature;
  DeviceAddress address_;
};

class DS18B20Mock : public DS18B20 {
 public:
  DS18B20Mock(const DeviceAddress& address = {0}) : DS18B20(address) {}
  virtual int8_t getTemperature() { return 25; }
};

#endif