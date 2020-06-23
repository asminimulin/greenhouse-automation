#pragma once

#include <stdint-gcc.h>

class Screen {
 public:
  Screen() = default;
  virtual void renderFirst() = 0;
  virtual void renderSecond() = 0;
  inline virtual void render() {
    renderFirst();
    renderSecond();
  }
  inline char* getFirstBuffer() { return data[0]; }
  inline char* getSecondBuffer() { return data[1]; }

  virtual ~Screen() {}

 protected:
  static constexpr const uint8_t rows = 2;
  static constexpr const uint8_t columns = 16;

 private:
  char data[rows][columns + 1];
};