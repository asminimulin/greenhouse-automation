#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <stdint.h>


namespace ns_encoder {

typedef void(*handler)();


void init();


// extern handler increment;
// extern handler decrement;
// extern handler incrementPress;
// extern handler decrementPress;

void increment();
void decrement();
void incrementPress();
void decrementPress();

}


#endif