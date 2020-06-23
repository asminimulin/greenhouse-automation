#pragma once

#include "menu/menu.hpp"
#include "screen.hpp"

class MenuScreen : public Screen {
 public:
  inline MenuScreen() {}

  inline void render() final {
    char* first = getFirstBuffer();
    char* second = getSecondBuffer();
    ns_menu::renderMenu(first, second);
  }

  inline void renderFirst() final {}

  inline void renderSecond() final {}

  ~MenuScreen() final {}
};