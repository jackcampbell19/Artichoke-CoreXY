#ifndef Artichoke_HEADER
#define Artichoke_HEADER


#include "vector.h"
#include <stdlib.h>
#include "stepper.h"
#include "paint.h"


typedef struct {
	Vector size;
	Vector position;
	Stepper *aStepper;
	Stepper *bStepper;
	Stepper *zStepper;
	Vector *limitSwitches;
	PaintDispenser *paintDispenser;
	int32_t cupHolderPosition;
} Artichoke;


bool artichokeValidatePosition(Artichoke *art, Vector *tpos);
void artichokeMoveSteppers(Artichoke *art, Vector *steps, uint64_t delay);
void artichokeComputeSteps(Vector *steps, Vector *delta);
bool artichokeSetPosition(Artichoke *art, Vector *tpos);
void artichokeHomeAxis(Artichoke *art);
void artichokeHomeAll(Artichoke *art);
void ejectCup(Artichoke *art);
void dispenseCup(Artichoke *art);
void setCupHolderPosition(Artichoke *art, int32_t position);
void homeCupHolder(Artichoke *art);
void homeCupDispenser();
void dispensePaint(Artichoke *art, Vector *color);
bool artichokeMoveRel(Artichoke *art, int32_t x, int32_t y, int32_t z, uint64_t delay, bool ignoreBounds);


#endif
