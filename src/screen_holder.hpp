#pragma once

#include <stdint-gcc.h>

#include <logging.hpp>

#include "idle.hpp"

template <uint8_t maxScreenCount>
class ScreenHolder {
 public:
  inline ScreenHolder() {
    screensCount_ = 0;
    activeScreen_ = 0;
  };

  inline void registerScreen(Screen* screen) {
    if (screensCount_ == maxScreenCount) {
      logging::error() << F(
          "Too many screens. Not enough space in screen holder.");
      idle();
    }
    screens_[screensCount_++] = screen;
  }

  inline Screen* getActiveScreen() {
    if (screensCount_ == 0) {
      return nullptr;
    }
    return screens_[activeScreen_];
  }

  void next() { activeScreen_ = (activeScreen_ + 1) % screensCount_; };

  void previous() {
    activeScreen_ = (screensCount_ + activeScreen_ - 1) % screensCount_;
  }

 private:
  Screen* screens_[maxScreenCount];
  uint8_t screensCount_;
  uint8_t activeScreen_;
};