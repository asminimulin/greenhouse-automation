#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <stdint.h>

namespace ns_encoder {

typedef void (*handler)();

void init();

void increment();
void decrement();
void incrementPress();
void decrementPress();

}  // namespace ns_encoder

#endif