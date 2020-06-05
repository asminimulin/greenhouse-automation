#include "screen.hpp"
#include "config.hpp"
#include "idle.hpp"
#include "logging/logging.hpp"


namespace {

static LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);


void fillSpaces(char* buffer) {
    memset(buffer, ' ', LCD_COLUMNS);
    buffer[LCD_COLUMNS] = '\0';
}


void fillSpaces(char* buffer, int start, int end) {
    if (end < start) return;
    memset(&buffer[start], ' ', end - start + 1);
    buffer[end] = '\0';
}


static char buffers[LCD_ROWS][LCD_COLUMNS + 1];


static uint32_t lastInterrupt;

}


namespace ns_screen {


ScreenLightSetting screenLightSetting;


void resetInterruptTimer() {
    lastInterrupt = millis();
}


void loop() {
    static constexpr uint32_t autoBacklighDisableTime = 20lu * 1000lu;
    lcd.setCursor(0, 0);
    int len = strlen(buffers[0]);
    fillSpaces(buffers[0], len, LCD_COLUMNS);
    lcd.printstr(buffers[0]);

    lcd.setCursor(0, 1);
    len = strlen(buffers[1]);
    fillSpaces(buffers[1], len, LCD_COLUMNS);
    lcd.printstr(buffers[1]);

    ns_screen::screenLightSetting = ScreenLightSetting(int8_t(screenLightSetting) % 3);

    if (screenLightSetting == LIGHT_ON) {
        if (!lcd.getBacklight()) {
            if (ENABLE_DEBUG_OUTPUT) {
                logging::debug(F("ScreenLightSetting: "));
                logging::debug(screenLightSetting);
            }
            lcd.backlight();
        }
    } else if (screenLightSetting == LIGHT_OFF) {
        if (lcd.getBacklight()) {
            if (ENABLE_DEBUG_OUTPUT) {
                logging::debug(F("ScreenLightSetting: "));
                logging::debug(screenLightSetting);
            }
            lcd.noBacklight();
        }
    } else {
        bool shouldOn = millis() - lastInterrupt <= autoBacklighDisableTime;
        if (shouldOn && !lcd.getBacklight()) {
            if (ENABLE_DEBUG_OUTPUT) {
                logging::debug(F("ScreenLightSetting: "));
                logging::debug(screenLightSetting);
            }
            lcd.backlight();
        } else if (!shouldOn && lcd.getBacklight()) {
            if (ENABLE_DEBUG_OUTPUT) {
                logging::debug(F("ScreenLightSetting: "));
                logging::debug(screenLightSetting);
            }
            lcd.noBacklight();
        }
    }
}


void init() {
    lcd.begin();
    lcd.noAutoscroll();
    lcd.noBlink();
    lcd.noCursor();
    lastInterrupt = 0;
    screenLightSetting = LIGHT_ON;
}


char* getWritableBuffer(uint8_t rowNumber) {
    if (rowNumber >= LCD_ROWS) {
        idle();
    }
    fillSpaces(buffers[rowNumber]);
    return buffers[rowNumber];
}


void screenLightSettingRepresenter(int8_t value, char* row) {
    value %= 3;
    char buffer[16];
    auto* on_ = reinterpret_cast<const char*>(F("Light on"));
    auto* off_ = reinterpret_cast<const char*>(F("Light off"));
    auto* auto_ = reinterpret_cast<const char*>(F("Light auto"));
    for (int i = 0; ; ++i) {
        char c;
        if (value == LIGHT_ON) {
            c = pgm_read_byte(i + on_);
        } else if (value == LIGHT_OFF) {
            c = pgm_read_byte(i + off_);
        } else {
            c = pgm_read_byte(i + auto_);
        }
        buffer[i] = c;
        if (c == '\0') {
            break;
        }
    }
    sprintf(row, "%s", buffer);
}


}