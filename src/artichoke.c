#include "artichoke.h"
#include "pico/stdlib.h"
#include "math.h"
#include "delay.h"
#include "constants.h"
#include "motors.h"


/**
 * Returns true of the vector "tpos" lies within the bounds of the
 * Artichoke machine, false otherwise.
*/
bool artichokeValidatePosition(Artichoke *art, Vector *tpos) {
	if (tpos->x < 0 || tpos->x > art->size.x) {
		return false;
	}
	if (tpos->y < 0 || tpos->y > art->size.y) {
		return false;
	}
	if (tpos->z < 0 || tpos->z > art->size.z) {
		return false;
	}
	return true;
}


/**
 * Moves the steppers of "art" in sync using the values of the "steps"
 * vector. Moves the a stepper x times, the b stepper y times, and the
 * z stepper z times.
*/
void artichokeMoveSteppers(Artichoke *art, Vector *steps, uint64_t delay) {
	gpio_put(art->aStepper->dir, steps->x >= 0);
	gpio_put(art->bStepper->dir, steps->y >= 0);
	gpio_put(art->zStepper->dir, steps->z >= 0);
	steps->x = abs(steps->x);
	steps->y = abs(steps->y);
	steps->z = abs(steps->z);
	while (steps->x > 0 || steps->y > 0 || steps->z > 0) {
		gpio_put(art->aStepper->stp, steps->x > 0);
		gpio_put(art->bStepper->stp, steps->y > 0);
		gpio_put(art->zStepper->stp, steps->z > 0);
		sleep_us(delay);
		gpio_put(art->aStepper->stp, false);
		gpio_put(art->bStepper->stp, false);
		gpio_put(art->zStepper->stp, false);
		sleep_us(delay);
		steps->x -= 1;
		steps->y -= 1;
		steps->z -= 1;
	}
}


/**
 * Computes the required number of steps/direction for each 
 * motor (a,b,z) to achieve the delta vector. Stores the result
 * in "steps".
*/
void artichokeComputeSteps(Vector *steps, Vector *delta) {
	steps->x = -(delta->y + delta->x);
	steps->y = delta->y - delta->x;
	steps->z = delta->z;
}


/**
 * Moves the Artichoke machine to the positon "tpos". Returns
 * true if the position is successful and false otherwise.
*/
bool artichokeSetPosition(Artichoke *art, Vector *tpos) {
	/**
	 * dx(x) = 1/2 (dx(a) + dx(b))
	 * dx(y) = 1/2 (dx(a) - dx(b))
	*/
	if (!artichokeValidatePosition(art, tpos)) {
		return false;
	}
	double length = vector_length(&art->position, tpos);
	size_t rounded_length = (size_t) ceil(length);
	if (rounded_length == 0) {
		return true;
	}
	Vector prevPos;
	vector_copy(&prevPos, &art->position);
	Vector rebasedVector;
	vector_copy(&rebasedVector, tpos);
	vector_subtract(&rebasedVector, &art->position);
	uint64_t (*delay_func)(double, double) = &calculate_delay;
	if (rebasedVector.x == 0 && rebasedVector.y == 0) {
		delay_func = &calculate_delay_fast;
	}
	for (size_t i = 0; i < rounded_length + 1; i++) {
		Vector delta;
		vector_copy(&delta, &rebasedVector);
		vector_multiply_floor(&delta, (((double) i) / ((double) rounded_length)));
		vector_add(&delta, &art->position);
		vector_subtract(&delta, &prevPos);
		if (vector_equals(&delta, 0, 0, 0)) {
			continue;
		}
		Vector steps;
		artichokeComputeSteps(&steps, &delta);
		artichokeMoveSteppers(art, &steps, delay_func(i, rounded_length + 1));
		vector_add(&prevPos, &delta);
	}
	art->position.x = tpos->x;
	art->position.y = tpos->y;
	art->position.z = tpos->z;
	return true;
}


bool artichokeMoveRel(Artichoke *art, int32_t x, int32_t y, int32_t z, uint64_t delay, bool ignoreBounds) {
	Vector position = {x, y, z};
	vector_add(&position, &art->position);
	if (!ignoreBounds && !artichokeValidatePosition(art, &position)) {
		return false;
	}
	Vector steps;
	Vector move = {x, y, z};
	artichokeComputeSteps(&steps, &move);
	artichokeMoveSteppers(art, &steps, delay);
	art->position.x += x;
	art->position.y += y;
	art->position.z += z;
	return true;
}


void artichokeHomeAxis(Artichoke *art) {
	Vector steps;
	while (!gpio_get(art->limitSwitches->z)) {
		artichokeMoveRel(art, 0, 0, -1, PULSE_DELAY_HOMING_Z, true);
	}
	artichokeMoveRel(art, 0, 0, HOME_OFFSET_Z, PULSE_DELAY_HOMING_Z, true);
	while (!gpio_get(art->limitSwitches->x)) {
		artichokeMoveRel(art, -1, 0, 0, PULSE_DELAY_HOMING_XY, true);
	}
	artichokeMoveRel(art, HOME_OFFSET_X_Y, 0, 0, PULSE_DELAY_HOMING_XY, true);
	while (!gpio_get(art->limitSwitches->y)) {
		artichokeMoveRel(art, 0, -1, 0, PULSE_DELAY_HOMING_XY, true);
	}
	artichokeMoveRel(art, 0, HOME_OFFSET_X_Y, 0, PULSE_DELAY_HOMING_XY, true);
	art->position.x = 0;
	art->position.y = 0;
	art->position.z = 0;
}


void artichokeHomeAll(Artichoke *art) {
	artichokeHomeAxis(art);
	homeCupHolder(art);
	homePaintDispenser(art->paintDispenser);
	homeCupDispenser();
}


void homeCupDispenser() {
	motorOn(MOTOR_CUP_DISPENSER, false);
	sleep_ms(2200);
}


void ejectCup(Artichoke *art) {

}


void dispensePaint(Artichoke *art, Vector *color) {
	if (vector_equals(art->paintDispenser->color, -1, -1, -1) || !vector_comp(art->paintDispenser->color, color)) {
		ejectCup(art);
		dispenseCup(art);
	}
	// todo: set position
	homePaintDispenser(art->paintDispenser);
	//...
}


/**
 * Dispenses a single cup from the cup dispenser.
*/
void dispenseCup(Artichoke *art) {
	motorOn(MOTOR_CUP_DISPENSER, true);
	sleep_ms(2200);
	motorOn(MOTOR_CUP_DISPENSER, false);
	sleep_ms(2200);
	motorsOff();
}


void waitForReflectanceValue(bool value) {
	while (gpio_get(PIN_REFLECTANCE) != value) {
		continue;
	}
}


/**
 * Sets the cup holder to one of the 3 positions.
*/
void setCupHolderPosition(Artichoke *art, int32_t position) {
	int32_t delta = position - art->cupHolderPosition;
	motorOn(MOTOR_CUP_HOLDER, delta >= 0);
	for (size_t i = 0; i < abs(delta); i++) {
		sleep_ms(1000);
		waitForReflectanceValue(false);
	}
	motorsOff();
	art->cupHolderPosition = position;
}


void homeCupHolder(Artichoke *art) {
	motorOn(MOTOR_CUP_HOLDER, false);
	sleep_ms(6000);
	motorOn(MOTOR_CUP_HOLDER, true);
	waitForReflectanceValue(false);
	motorsOff();
	art->cupHolderPosition = 0;
}