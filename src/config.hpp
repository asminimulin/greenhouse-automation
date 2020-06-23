#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>

/************************* Блокировщик ***********************/
/* Теплица перестанет работать, когда отработает в течение такого времени:
 * BLOCKER_BLOCK_SYSTERM_AFTER_TIME = BLOCKER_BLOCK_AFTER_MONTHS *
 * BLOCKER_MONTH_TIME + BLOCKER_BLOCK_AFTER_WEEKS * BLOCKER_WEEK_TIME
 * ONE_DAY_TIME = 1000ms * 60s * 60m * 24h
 * BLOCKER_MONTH_TIME = 30 * ONE_DAY_TIME
 * BLOCKER_WEEK_TIME = 7 * ONE_DAY_TIME
 */

// Читать сколько времени от периода аренды уже прошло из EEPROM?
constexpr bool BLOCKER_USE_EEPROM = true;

// EEPROM Адрес настроек блокировщика
constexpr uint16_t EEPROM_BLOCKER_TIMER_ADDRESS = 100;

// Мы настраиваем перед тем как сдать в аренду
constexpr uint32_t BLOCKER_BLOCK_AFTER_MONTHS = 0;
constexpr uint32_t BLOCKER_BLOCK_AFTER_WEEKS = 1;
constexpr uint32_t BLOCKER_BLOCK_AFTER_MINUTES = 0;

/***************************************************************/

/************************* Первая теплица **********************/
// Адрес настроек в EEPROM
constexpr uint16_t EEPROM_PROPERTIES_ADDRESS = 200;

// Адреса

constexpr uint8_t YELLOW_SENSOR_ADDRESS[8] = {0x28, 0xFF, 0x06, 0xA3,
                                              0x31, 0x18, 0x01, 0x44};
constexpr uint8_t GREEN_SENSOR_ADDRESS[8] = {0x28, 0xFF, 0x96, 0xB0,
                                             0x31, 0x18, 0x01, 0x43};
constexpr uint8_t OUTSIDE_SENSOR_ADDRESS[8] = {0x28, 0xB9, 0x8B, 0x16,
                                               0xA8, 0x01, 0x3C, 0x0F};
constexpr uint8_t YELLOW_MOTOR_ADDRESS[8] = {0xBA, 0xCD, 0xC5, 0x73,
                                             0x50, 0x05, 0x10, 0x14};
constexpr uint8_t GREEN_MOTOR_ADDRESS[8] = {0xBA, 0x59, 0xD4, 0x73,
                                            0x50, 0x05, 0x10, 0xC8};
constexpr uint8_t VENT_ADDRESS[8] = {0};

constexpr uint32_t OPENING_TIME = 4LU * 1000LU;
constexpr uint32_t TEMPERATURE_INNERCY_DELAY = 0.5 * 60LU * 1000LU;

/*****************************************************************/

// Моторы
// Время работы привода от состояния полность закрыто до полностью открыто
constexpr uint32_t MOTOR_OPENING_TIME = 4 * 1000lu;

// ONEWIRE
constexpr uint8_t ONE_WIRE_PIN = 5;

// Энкодер
constexpr uint8_t ENCODER_INTERRUPT_PIN = 2;
constexpr uint8_t ENCODER_DIRECTION_PIN = 6;
constexpr uint8_t ENCODER_PRESS_PIN = 4;

// Экран
constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t LCD_COLUMNS = 16;

// Настройки
// Гисетерзис температуры открытия и закрытия
constexpr uint8_t HISTERESIS = 2;

// Печатать отладочную информацию?
constexpr bool ENABLE_DEBUG_OUTPUT = false;

#endif
