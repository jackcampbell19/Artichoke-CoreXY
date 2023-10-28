#ifndef STEPPER_HEADER
#define STEPPER_HEADER

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t stp;
	uint32_t dir;
	uint32_t en;
	int32_t current;
} Stepper;

void stepperMoveAbs(Stepper *stepper, int32_t targetStep, uint64_t delay);
void stepperMoveRel(Stepper *stepper, int32_t steps, bool direction, uint64_t delay);

#endif