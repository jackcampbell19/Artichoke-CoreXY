#ifndef COREXY_HEADER
#define COREXY_HEADER

#include "vector.h"
#include <stdlib.h>


typedef struct {
	uint32_t stp;
	uint32_t dir;
	uint32_t en;
} Stepper;


typedef struct {
	Stepper *horizontalStepper;
	Stepper *verticalStepper;
	uint32_t limitSwitch;
	uint32_t electromagnet;
	Vector *position;
} PaintDispenser;


typedef struct {
	Vector size;
	Vector position;
	Stepper *aStepper;
	Stepper *bStepper;
	Stepper *zStepper;
	Vector *limitSwitches;
	PaintDispenser *paintDispenser;
	int32_t cupHolderPosition;
} CoreXY;


bool validatePosition(CoreXY *cxy, Vector *tpos);
void setStepperDirection(Stepper *stepper, int32_t direction);
void moveSteppers(CoreXY *cxy, Vector *steps, uint64_t delay);
void computeSteps(Vector *steps, Vector *delta);
bool setPosition(CoreXY *cxy, Vector *tpos);


#endif
