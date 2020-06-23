#include "display.hpp"

#include "config.hpp"
#include "idle.hpp"
#include "logging/logging.hpp"
#include "logging/logging2.hpp"

namespace {

void fillSpaces(char* buffer) {
  memset(buffer, ' ', LCD_COLUMNS);
  buffer[LCD_COLUMNS] = '\0';
}

void fillSpaces(char* buffer, int start, int end) {
  if (end <= start) return;
  memset(&buffer[start], ' ', end - start);
  buffer[end - 1] = '\0';
}

}  // namespace

namespace ns_display {

void screenLightSettingRepresenter(int8_t value, char* row) {
  value %= 3;
  char buffer[16];
  auto* on_ = reinterpret_cast<PGM_P>(F("Light on"));
  auto* off_ = reinterpret_cast<PGM_P>(F("Light off"));
  auto* auto_ = reinterpret_cast<PGM_P>(F("Light auto"));
  if (value == Display::LIGHT_ON) {
    strcpy_P(buffer, on_);
  } else if (value == Display::LIGHT_OFF) {
    strcpy_P(buffer, off_);
  } else {
    strcpy_P(buffer, auto_);
  }
  sprintf(row, "%s", buffer);
}  // namespace ns_display

}  // namespace ns_display

Display::Display(uint8_t lcdAddres)
    : lcd_(lcdAddres, columnsCount_, rowsCount_) {}

bool Display::begin() {
  lcd_.begin();
  lcd_.backlight();
  lastInterruptTime_ = 0;
  screenLightingSetting_ = LIGHT_AUTO;
  lastInterruptTime_ = 0;

  uint8_t degreeSymbol[] = {
      0b00000, 0b00110, 0b01001, 0b01001, 0b00110, 0b00000, 0b00000, 0b00000,
  };

  lcd_.createChar(SYMBOL_DEGREE, degreeSymbol);
  return true;
}

void Display::loop() {
  if (screen_ != nullptr) {
    char buffer[columnsCount_ + 1];
    screen_->render();

    lcd_.home();
    size_t len = strlen(strcpy(buffer, screen_->getFirstBuffer()));
    fillSpaces(buffer, len, columnsCount_ + 1);
    lcd_.print(buffer);

    lcd_.setCursor(0, 1);
    len = strlen(strcpy(buffer, screen_->getSecondBuffer()));
    fillSpaces(buffer, len, columnsCount_ + 1);
    lcd_.print(buffer);
  }

  screenLightingSetting_ =
      ScreenLightingSetting(int8_t(screenLightingSetting_) % 3);

  if (screenLightingSetting_ == LIGHT_ON) {
    if (!lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.backlight();
    }
  } else if (screenLightingSetting_ == LIGHT_OFF) {
    if (lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.noBacklight();
    }
  } else {
    bool shouldOn = millis() - lastInterruptTime_ <= backlightAutoDisableTime_;
    if (shouldOn && !lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.backlight();
    } else if (!shouldOn && lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.noBacklight();
    }
  }
}

/*void Display::loop() {
  lcd_.setCursor(0, 0);
  int len = strlen(data_[0]);
  fillSpaces(data_[0], len, LCD_COLUMNS);
  lcd_.printstr(data_[0]);

  lcd_.setCursor(0, 1);
  len = strlen(data_[1]);
  fillSpaces(data_[1], len, LCD_COLUMNS);
  lcd_.printstr(data_[1]);

  screenLightingSetting_ =
      ScreenLightingSetting(int8_t(screenLightingSetting_) % 3);

  if (screenLightingSetting_ == LIGHT_ON) {
    if (!lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.backlight();
    }
  } else if (screenLightingSetting_ == LIGHT_OFF) {
    if (lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.noBacklight();
    }
  } else {
    bool shouldOn = millis() - lastInterruptTime_ <= backlightAutoDisableTime_;
    if (shouldOn && !lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.backlight();
    } else if (!shouldOn && lcd_.getBacklight()) {
      logging2::info() << F("ScreenLightSetting: ") << screenLightingSetting_;
      lcd_.noBacklight();
    }
  }
}*/

void Display::resetInterruptTimer() { lastInterruptTime_ = millis(); }
