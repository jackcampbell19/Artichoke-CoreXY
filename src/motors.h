#ifndef MOTORS_HEADER
#define MOTORS_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "artichoke.h"

void activate_motor(uint32_t motorIndex, bool direction);
void deactivate_motors();

#endif