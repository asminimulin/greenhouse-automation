#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

/* Software configuration */

constexpr bool ENABLE_DEBUG_OUTPUT = false;

constexpr uint16_t EEPROM_PROPERTIES_ADDRESS = 200;

constexpr uint8_t YELLOW_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t GREEN_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t OUTSIDE_SENSOR_ADDRESS[8] = {0};

constexpr uint8_t YELLOW_MOTOR_ADDRESS[8] = {0};

constexpr uint8_t GREEN_MOTOR_ADDRESS[8] = {0};

constexpr uint8_t VENT_ADDRESS[8] = {0};

constexpr uint32_t OPENING_TIME = 4LU * 1000LU;

constexpr uint32_t TEMPERATURE_INNERCY_DELAY = 0.25 * 60LU * 1000LU;

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
