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


/**
 * Returns true of the vector "tpos" lies within the bounds of the
 * Artichoke machine, false otherwise.
*/
bool _validate_position(Artichoke *art, Vector *tpos);


/**
 * Moves the steppers of "art" in sync using the values of the "steps"
 * vector. Moves the a stepper x times, the b stepper y times, and the
 * z stepper z times.
*/
void _move_steppers(Artichoke *art, Vector *steps, uint64_t delay);


/**
 * Computes the required number of steps/direction for each 
 * motor (a,b,z) to achieve the delta vector. Stores the result
 * in "steps".
*/
void _compute_steps(Vector *steps, Vector *delta);


/**
 * Moves the Artichoke machine to the positon "tpos". Returns
 * true if the position is successful and false otherwise.
*/
bool artichoke_set_position(Artichoke *art, Vector *tpos);


void home_axis(Artichoke *art);


void home_all(Artichoke *art);


void eject_cup(Artichoke *art);

/**
 * Dispenses a single cup from the cup dispenser.
*/
void dispense_cup(Artichoke *art);

/**
 * Sets the cup holder to one of the 3 positions.
*/
void set_cup_holder_position(Artichoke *art, int32_t position);


void home_cup_holder(Artichoke *art);


void home_cup_dispenser();


void dispense_paint(Artichoke *art, Vector *color);


bool move_axis_rel(Artichoke *art, int32_t x, int32_t y, int32_t z, uint64_t delay, bool ignoreBounds);


#endif
