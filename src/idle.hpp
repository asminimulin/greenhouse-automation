#ifndef IDLE_HPP
#define IDLE_HPP
#include <Arduino.h>

#include "logging/logging.hpp"


inline void idle() {
    logging::error(F("idle()"));
    interrupts();
    while(1) {
        logging::error(F("idle()"));
        delay(5000lu);
    }
}

#endif