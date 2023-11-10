#ifndef Artichoke_HEADER
#define Artichoke_HEADER


#include "vector.h"
#include <stdlib.h>
#include "stepper.h"
#include "paint.h"
#include "constants.h"


typedef struct {
	Vector size;
	Vector position;
	Stepper *aStepper;
	Stepper *bStepper;
	Stepper *zStepper;
	Vector *limitSwitches;
	PaintDispenser *paintDispenser;
	int32_t cupHolderPosition;
	uint8_t toolIndex;
	bool hasCup;
	uint8_t *color;
} Artichoke;


/**
 * Moves the Artichoke machine to the positon "tpos". Returns
 * true if the position is successful and false otherwise.
*/
bool artichoke_move_line(Artichoke *art, Vector *tpos, bool fast);


/**
 * Moves the gantry along an arc.
*/
bool artichoke_move_arc(Artichoke *art, Vector *center, Vector *v, double rotationDeg, bool fast);


/**
 * Homes the 3 axis of the gantry.
*/
void home_axis(Artichoke *art);


/**
 * Homes all components in the proper order.
*/
void home_all(Artichoke *art);


/**
 * Sets the cup holder to one of the 3 positions.
*/
void set_cup_holder_position(Artichoke *art, int32_t position, bool moveTool);


void home_cup_holder(Artichoke *art);


void home_cup_dispenser();


void dispense_paint(Artichoke *art, uint8_t color[COLOR_BUFFER_SIZE]);


bool move_axis_rel(Artichoke *art, int32_t x, int32_t y, int32_t z, uint64_t delay, bool ignoreBounds);


bool load_tool(Artichoke *art, uint8_t toolIndex);


#endif
