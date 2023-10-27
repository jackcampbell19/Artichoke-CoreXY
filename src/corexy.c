#include "corexy.h"
#include "pico/stdlib.h"
#include "math.h"
#include "delay.h"
#include "constants.h"

/**
 * Returns true of the vector "tpos" lies within the bounds of the
 * CoreXY machine, false otherwise.
*/
bool validatePosition(CoreXY *cxy, Vector *tpos) {
	if (tpos->x < 0 || tpos->x > cxy->size.x) {
		return false;
	}
	if (tpos->y < 0 || tpos->y > cxy->size.y) {
		return false;
	}
	if (tpos->z < 0 || tpos->z > cxy->size.z) {
		return false;
	}
	return true;
}


/**
 * Pulls the dir pin of the stepper high when direction is >= 0,
 * pulls low otherwise.
*/
void setStepperDirection(Stepper *stepper, int32_t direction) {
	gpio_put(stepper->dir, direction >= 0);
}


/**
 * Moves the steppers of "cxy" in sync using the values of the "steps"
 * vector. Moves the a stepper x times, the b stepper y times, and the
 * z stepper z times.
*/
void moveSteppers(CoreXY *cxy, Vector *steps, uint64_t delay) {
	setStepperDirection(cxy->aStepper, steps->x);
	setStepperDirection(cxy->bStepper, steps->y);
	setStepperDirection(cxy->zStepper, steps->z);
	steps->x = abs(steps->x);
	steps->y = abs(steps->y);
	steps->z = abs(steps->z);
	while (steps->x > 0 || steps->y > 0 || steps->z > 0) {
		gpio_put(cxy->aStepper->stp, steps->x > 0);
		gpio_put(cxy->bStepper->stp, steps->y > 0);
		gpio_put(cxy->zStepper->stp, steps->z > 0);
		sleep_us(delay);
		gpio_put(cxy->aStepper->stp, false);
		gpio_put(cxy->bStepper->stp, false);
		gpio_put(cxy->zStepper->stp, false);
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
void computeSteps(Vector *steps, Vector *delta) {
	steps->x = -(delta->y + delta->x);
	steps->y = delta->y - delta->x;
	steps->z = delta->z;
}


/**
 * Moves the CoreXY machine to the positon "tpos". Returns
 * true if the position is successful and false otherwise.
*/
bool setPosition(CoreXY *cxy, Vector *tpos) {
	/**
	 * dx(x) = 1/2 (dx(a) + dx(b))
	 * dx(y) = 1/2 (dx(a) - dx(b))
	*/
	if (!validatePosition(cxy, tpos)) {
		return false;
	}
	double length = vector_length(&cxy->position, tpos);
	size_t rounded_length = (size_t) ceil(length);
	if (rounded_length == 0) {
		return true;
	}
	Vector prevPos;
	vector_copy(&prevPos, &cxy->position);
	Vector rebasedVector;
	vector_copy(&rebasedVector, tpos);
	vector_subtract(&rebasedVector, &cxy->position);
	for (size_t i = 0; i < rounded_length + 1; i++) {
		Vector delta;
		vector_copy(&delta, &rebasedVector);
		vector_multiply_floor(&delta, (((double) i) / ((double) rounded_length)));
		vector_add(&delta, &cxy->position);
		vector_subtract(&delta, &prevPos);
		if (vector_equals(&delta, 0, 0, 0)) {
			continue;
		}
		Vector steps;
		computeSteps(&steps, &delta);
		moveSteppers(cxy, &steps, calculate_delay(i, rounded_length + 1));
		vector_add(&prevPos, &delta);
	}
	cxy->position.x = tpos->x;
	cxy->position.y = tpos->y;
	cxy->position.z = tpos->z;
	return true;
}
