#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdint.h>




// ONEWIRE
constexpr uint8_t ONE_WIRE_PIN = 5;




/************************* Блокировщик ***********************/
/* Теплица перестанет работать, когда отработает в течение такого времени:
 * BLOCKER_BLOCK_SYSTERM_AFTER_TIME = BLOCKER_BLOCK_AFTER_MONTHS * BLOCKER_MONTH_TIME +
 *                                   BLOCKER_BLOCK_AFTER_WEEKS * BLOCKER_WEEK_TIME
 * ONE_DAY_TIME = 1000ms * 60s * 60m * 24h
 * BLOCKER_MONTH_TIME = 30 * ONE_DAY_TIME
 * BLOCKER_WEEK_TIME = 7 * ONE_DAY_TIME
 */

// Читать сколько времени от периода аренды уже прошло из EEPROM?
constexpr bool BLOCKER_USE_EEPROM = false;

// EEPROM Адрес настроек блокировщика
constexpr uint16_t EEPROM_BLOCKER_TIMER_ADDRESS = 100;


// Мы настраиваем перед тем как сдать в аренду
constexpr uint32_t BLOCKER_BLOCK_AFTER_MONTHS = 0;
constexpr uint32_t BLOCKER_BLOCK_AFTER_WEEKS = 1;


/***************************************************************/





/************************* Первая теплица **********************/
// Адрес настроек в EEPROM
constexpr uint16_t EEPROM_FIRST_GREENHOUSE_PROPERTIES_ADDRESS = 200;


// Адреса
constexpr uint8_t FIRST_GREENHOUSE_YELLOW_SENSOR_ADDRESS[8]    = {0};
constexpr uint8_t FIRST_GREENHOUSE_GREEN_SENSOR_ADDRESS[8]     = {0};
constexpr uint8_t FIRST_GREENHOUSE_OUTSIDE_SENSOR_ADDRESS[8]   = {0};
constexpr uint8_t FIRST_GREENHOUSE_YELLOW_MOTOR_ADDRESS[8]     = {0};
constexpr uint8_t FIRST_GREENHOUSE_GREEN_MOTOR_ADDRESS[8]      = {0};
constexpr uint8_t FIRST_GREENHOUSE_VENT_ADDRESS[8]             = {0};

/*****************************************************************/





/************************* Вторая теплица **********************/
constexpr bool HAS_SECOND_GREENHOUSE = false;
constexpr uint16_t EEPROM_SECOND_GREENHOUSE_PROPERTIES_ADDRESS = 300;

// Адреса
constexpr uint8_t SECOND_GREENHOUSE_YELLOW_SENSOR_ADDRESS[8]   = {0};
constexpr uint8_t SECOND_GREENHOUSE_GREEN_SENSOR_ADDRESS[8]    = {0};
constexpr uint8_t SECOND_GREENHOUSE_OUTSIDE_SENSOR_ADDRESS[8]  = {0};
constexpr uint8_t SECOND_GREENHOUSE_YELLOW_MOTOR_ADDRESS[8]    = {0};
constexpr uint8_t SECOND_GREENHOUSE_GREEN_MOTOR_ADDRESS[8]     = {0};
constexpr uint8_t SECOND_GREENHOUSE_VENT_ADDRESS[8]            = {0};

/*****************************************************************/




// Моторы
// Время работы привода от состояния полность закрыто до полностью открыто
constexpr uint32_t MOTOR_OPENING_TIME = 40 * 1000lu;





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
