#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

/* Software configuration */

constexpr bool ENABLE_DEBUG_OUTPUT = false;

constexpr uint16_t SETTINGS_EEPROM_POSITION = 200;
// This value is only need to check if settings were saved in EEPROM previously
const uint8_t SETTINGS_EEPROM_PROTECTION_VALUE = 0x01;

constexpr uint16_t ONE_WIRE_ADDRESSES_EEPROM_POSITION = 300;
// This value is only need to check if addresses were saved in EEPROM previously
constexpr uint8_t ONE_WIRE_ADDRESSES_EEPROM_PROTECTION_VALUE = 0x11;

constexpr uint8_t YELLOW_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t GREEN_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t OUTSIDE_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t YELLOW_MOTOR_ADDRESS[8] = {0};

constexpr uint8_t GREEN_MOTOR_ADDRESS[8] = {0};

constexpr uint8_t VENT_ADDRESS[8] = {0};

constexpr uint32_t OPENING_TIME = 38LU * 1000LU;

constexpr uint32_t TEMPERATURE_INNERCY_DELAY = 4 * 60LU * 1000LU;

constexpr uint8_t HISTERESIS = 2;

/* Hardware configuration */

constexpr uint8_t ONE_WIRE_PIN = 5;

constexpr uint8_t ENCODER_INTERRUPT_PIN = 2;
constexpr uint8_t ENCODER_DIRECTION_PIN = 6;
constexpr uint8_t ENCODER_PRESS_PIN = 4;

constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t LCD_COLUMNS = 16;

#endif
