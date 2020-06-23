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

class Greenhouse {
  friend void buildGreenhouseMenu();
  friend bool validateOpeningTemperature(int8_t temp);
  friend bool validateClosingTemperature(int8_t temp);
  friend bool validateOpeningSteps(int8_t steps);
  friend void buildSummerModeMenu();
  friend void ventModeRepresenter(int8_t value, char* buffer);

 public:
  Greenhouse(const GreenhouseConfig& config, uint16_t settingsPosition);
  bool begin();
  void loadSettings();
  void saveSettings();
  void loop();
  static void getTempRepresentation(int8_t temperature, char* buffer);
  int8_t getOutsideTemperature() { return outsideSensor_.getTemperature(); }
  int8_t getYellowTemperature() { return yellowSensor_.getTemperature(); }
  int8_t getGreenTemperature() { return greenSensor_.getTemperature(); }
  bool getSummerMode() const noexcept { return bool(summerMode & 1); }
  void setSummeMode(bool enabled) noexcept { summerMode = enabled; }
  inline bool getVentStatus() noexcept { return vent_.getState() == VENT_ON; }
  inline uint8_t getYellowPerCent() const noexcept {
    return yellowWindow_.getPerCent();
  }
  inline uint8_t getGreenPerCent() const noexcept {
    return greenWindow_.getPerCent();
  }
  inline int8_t getOpeningTemperature() const noexcept {
    return openingTemperature;
  }
  inline int8_t getClosingTemperature() const noexcept {
    return closingTemperature;
  }
  inline uint8_t getStepsCount() const noexcept { return openingSteps; }

 private:
  uint32_t getOneStepTime() const { return openingTime / openingSteps; }

  Window yellowWindow_;
  Window greenWindow_;
  DS18B20 yellowSensor_;
  DS18B20 greenSensor_;
  DS18B20 outsideSensor_;
  DS2413 vent_;

  // Not user avialable properties
  uint32_t openingTime = 40LU * 1000LU;
  uint32_t temperatureInnercyDelay = 0.25 * 60LU * 1000LU;

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

  // User available properties
 private:
  int8_t openingTemperature = 24;
  int8_t closingTemperature = 20;
  uint8_t openingSteps = 6;
  uint8_t summerMode;

  // Vent
 private:
  static constexpr int8_t ventOnTemperature = 25;
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