#ifndef MOTORS_HEADER
#define MOTORS_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "corexy.h"

void motorOn(uint32_t motorIndex, bool direction);
void motorsOff();

void dispenseCup();
void setCupHolderPosition(CoreXY *cxy, int32_t position);
void homeCupHolder();

#endif