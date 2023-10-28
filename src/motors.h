#ifndef MOTORS_HEADER
#define MOTORS_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "artichoke.h"

void motorOn(uint32_t motorIndex, bool direction);
void motorsOff();

#endif