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

void homePaintDispenser(PaintDispenser *pd);
void movePaintDispenserHead(PaintDispenser *pd, int32_t syringe);

#endif
