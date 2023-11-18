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
} PaintDispenser;

void home_paint_dispenser(PaintDispenser *pd);
void paint_dispenser_dispense_quantity(PaintDispenser *pd, int32_t syringe, uint8_t quantity);

#endif
