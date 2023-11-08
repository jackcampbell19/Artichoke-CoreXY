#ifndef PAINT_HEADER
#define PAINT_HEADER

#include <stdint.h>
#include "stepper.h"
#include "vector.h"

typedef struct {
	Stepper *horizontalStepper;
	Stepper *verticalStepper;
	uint32_t limitSwitch;
	uint32_t electromagnet;
	Vector *color;
} PaintDispenser;

void home_paint_dispenser(PaintDispenser *pd);
void move_paint_dispenser_head(PaintDispenser *pd, int32_t syringe);

#endif
