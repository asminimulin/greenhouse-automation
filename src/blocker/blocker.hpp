#ifndef BLOCKER_HPP
#define BLOCKER_HPP

#include <stdint.h>
#include <logging/logging.hpp>


namespace ns_blocker {

void init();


void updateTime();


bool isBlocked();


uint32_t getRemainingTime();

}

#endif