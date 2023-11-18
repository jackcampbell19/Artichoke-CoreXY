#ifndef DELAY_HEADER
#define DELAY_HEADER

#include <stdint.h>
#include <stdlib.h>


typedef struct {
    double ramp;
    double max;
    double min;
} Speed;


uint64_t ease_delay_us(double current, double total, Speed *speed);


#endif