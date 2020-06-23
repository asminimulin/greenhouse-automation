#pragma once

#include "greenhouse/greenhouse.hpp"
#include "screen.hpp"

class GreenWindowScreen : public Screen {
 public:
  GreenWindowScreen(Greenhouse* greenhouse);
  void renderFirst() final;
  void renderSecond() final;
  void temperatureToString(char* buffer, int8_t temperature);

 private:
  Greenhouse* greenhouse_;
};