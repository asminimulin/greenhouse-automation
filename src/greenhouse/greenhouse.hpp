#ifndef GREENHOUSE_HPP
#define GREENHOUSE_HPP

#include "ds18b20.hpp"
#include "ds2413.hpp"
#include "config.hpp"


struct GreenhouseConfig {

DeviceAddress yellowSensorAddress;
DeviceAddress greenSensorAddress;
DeviceAddress outsideSensorAddress;
DeviceAddress yellowMotorAddress;
DeviceAddress greenMotorAddress;
DeviceAddress ventAddress;

};


class Greenhouse {
    friend void buildFirstGreenhouseMenu();
    friend void buildSecondGreenhouseMenu();
    friend bool validateFirstOpeningTemperature(int8_t temp);
    friend bool validateFirstClosingTemperature(int8_t temp);
    friend bool validateFirstOpeningSteps(int8_t steps);
    friend bool validateSecondOpeningTemperature(int8_t temp);
    friend bool validateSecondClosingTemperature(int8_t temp);
    friend bool validateSecondOpeningSteps(int8_t steps);

public:
    Greenhouse(const GreenhouseConfig& config, uint16_t settingsPosition);
    void loadSettings();
    void saveSettings();
    void loop();
    static void getTempRepresentation(int8_t temperature, char* buffer);
    int8_t getOutsideTemperature() { return outsideSensor.getTemperature(); }
    int8_t getYellowTemperature() { return yellowSensor.getTemperature(); }
    int8_t getGreenTemperature() { return greenSensor.getTemperature(); }


private:
    uint32_t getOneStepTime() const {
        return MOTOR_OPENING_TIME / openingSteps;
    }

    
private:
    DS2413 yellowMotor;
    DS2413 greenMotor;
    DS18B20 yellowSensor;
    DS18B20 greenSensor;
    DS18B20 outsideSensor;
    DS2413 vent_;

    // Configurable properties
    int8_t openingTemperature = 24;
    int8_t closingTemperature = 20;
    uint8_t openingSteps = 4;


// Motor
private:
    enum MOTOR_STATE {
        OPENINIG = 0b10,
        CLOSING = 0b01,
        STOPPED = 0b11,
    };

// Unconfigurable properties    
private:
    static constexpr int8_t outsideMotorEnablingTemperature = 15;
    static constexpr int8_t criticalHighTemperature = 35;
    static constexpr int8_t criticalLowTemperature = 15;


// EEPROM specific
private:
    enum {
        NO_VALID_DATA = 0,
        HAS_VALID_DATA = 1
    };
    uint16_t settingsPosition_;


// Vent
private:
    static constexpr int8_t ventOnTemperature = 25;
    enum VentState {
        VENT_ON = 0b11,
        VENT_OFF = 0b00,
    };
};

#endif