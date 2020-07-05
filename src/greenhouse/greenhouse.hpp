#ifndef GREENHOUSE_HPP
#define GREENHOUSE_HPP

#include "config.hpp"
#include "ds18b20.hpp"
#include "ds2413.hpp"
#include "window.hpp"

struct GreenhouseConfig {
  DeviceAddress yellowSensorAddress;
  DeviceAddress greenSensorAddress;
  DeviceAddress outsideSensorAddress;
  DeviceAddress yellowMotorAddress;
  DeviceAddress greenMotorAddress;
  DeviceAddress ventAddress;
  uint32_t openingTime;
  uint32_t temperatureInnercyDelay;
};

namespace ns_greenhouse {

#pragma pack(push, 1)
struct settings_t {
  uint64_t openingTime = OPENING_TIME;
  uint64_t temperatureInnercyDelay = TEMPERATURE_INNERCY_DELAY;
  int8_t openingTemperature = 30;
  int8_t closingTemperature = 25;
  int8_t ventOnTemperature = 20;
  uint8_t stepsCount = 4;
  uint8_t summerMode = 0;
  uint8_t ventMode = 0;
};
#pragma pack(pop)

}  // namespace ns_greenhouse

class Greenhouse {
  friend void buildGreenhouseMenu();
  friend bool validateOpeningTemperature(int8_t temp);
  friend bool validateClosingTemperature(int8_t temp);
  friend bool validateOpeningSteps(int8_t steps);
  friend void buildSummerModeMenu();
  friend void ventModeRepresenter(int8_t value, char* buffer);

  using settings_t = ns_greenhouse::settings_t;

 public:
  Greenhouse(const GreenhouseConfig& config, uint16_t settingsPosition);
  bool begin();
  void loadSettings();
  void saveSettings();
  void loop();
  int8_t getOutsideTemperature() noexcept;
  int8_t getYellowTemperature() noexcept;
  int8_t getGreenTemperature() noexcept;
  bool isVentEnabled() const noexcept;

  uint8_t getYellowPerCent() const noexcept;
  uint8_t getGreenPerCent() const noexcept;

  bool getSummerMode() const noexcept;
  void setSummeMode(bool enabled) noexcept;

  void setOpeningTemperature(int8_t openingTemperature) noexcept;
  int8_t getOpeningTemperature() const noexcept;

  void setClosingTemperature(int8_t closingTemperature) noexcept;
  int8_t getClosingTemperature() const noexcept;

  void setStepsCount(uint8_t stepsCount) noexcept;
  uint8_t getStepsCount() const noexcept;

  const settings_t& getSettingsReference() const noexcept;
  settings_t getSettings() const noexcept;
  void setSettings(const settings_t& settings) noexcept;

 public:
  /**
   * Hardware configuration methods
   */
  void setYellowSensorAddress(uint8_t* address);
  void setGreenSensorAddress(uint8_t* address);
  void setOutsideSensorAddress(uint8_t* address);
  void setYellowWindowAddress(uint8_t* address);
  void setGreenWindowAddress(uint8_t* address);
  void setVentAddress(uint8_t* address);

 private:
  uint32_t getOneStepTime() const noexcept;

  settings_t settings_;

  Window yellowWindow_;
  Window greenWindow_;
  DS18B20 yellowSensor_;
  DS18B20 greenSensor_;
  DS18B20 outsideSensor_;
  DS2413 vent_;

  // Motor
  uint32_t yellowWindowStateChangedAt = 0;
  uint32_t greenWindowStateChangedAt = 0;

  // EEPROM specific
 private:
  enum {
    NO_VALID_DATA = 0,
    HAS_VALID_DATA = 1,
  };
  uint16_t settingsPosition_;

  // Vent
 private:
  enum VentState {
    VENT_ON = 0b10,
    VENT_OFF = 0b00,
  };

  enum VentMode : uint8_t {
    VENT_DISABLE = 0,
    VENT_ENABLE = 1,
    VENT_AUTO = 2,
  };
  int8_t ventMode_;

  // Unconfigurable properties
 private:
  static constexpr int8_t outsideMotorEnablingTemperature = 15;
  static constexpr int8_t criticalHighTemperature = 35;
  static constexpr int8_t criticalLowTemperature = 15;
};

#endif