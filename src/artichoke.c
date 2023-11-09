#include "artichoke.h"
#include "pico/stdlib.h"
#include <math.h>
#include "delay.h"
#include "constants.h"
#include "motors.h"


uint32_t _TOOL_POSITIONS[8] = {1415, 2475, 3565, 4000, 5000, 6000, 7000, 8000};


bool _validate_position(Artichoke *art, Vector *tpos) {
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


void _move_steppers(Artichoke *art, Vector *steps, uint64_t delay) {
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


void _compute_steps(Vector *steps, Vector *delta) {
	steps->x = -(delta->y + delta->x);
	steps->y = delta->y - delta->x;
	steps->z = delta->z;
}


bool artichoke_move_line(Artichoke *art, Vector *tpos, bool fast) {
	/**
	 * dx(x) = 1/2 (dx(a) + dx(b))
	 * dx(y) = 1/2 (dx(a) - dx(b))
	*/
	if (!_validate_position(art, tpos)) {
		return false;
	}
	double length = vector_distance(&art->position, tpos);
	size_t rounded_length = (size_t) ceil(length);
	if (rounded_length == 0) {
		return true;
	}
	Vector prevPos;
	vector_copy(&prevPos, &art->position);
	Vector rebasedVector;
	vector_copy(&rebasedVector, tpos);
	vector_subtract(&rebasedVector, &art->position);
	uint64_t (*delay_func)(double, double) = fast ? &delay_fast_us : &delay_default_us;
	if (rebasedVector.x == 0 && rebasedVector.y == 0) {
		delay_func = &delay_z_us;
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
		_compute_steps(&steps, &delta);
		_move_steppers(art, &steps, delay_func(i, rounded_length + 1));
		vector_add(&prevPos, &delta);
	}
	art->position.x = tpos->x;
	art->position.y = tpos->y;
	art->position.z = tpos->z;
	return true;
}


bool artichoke_move_arc(Artichoke *art, Vector *center, Vector *v, double rotationDeg, bool fast) {
	double radius = vector_length(v);
	size_t samples = ((size_t) 2 * M_PI * radius);
	for (size_t i = 0; i < samples; i++) {
		Vector sample;
		vector_copy(&sample, v);
		double sampleAngle = (((double) i) / ((double) samples)) * rotationDeg;
		vector_rotate_floor(&sample, sampleAngle);
		vector_add(&sample, center);
		artichoke_move_line(art, &sample, fast);
	}
	return true;
}


bool move_axis_rel(Artichoke *art, int32_t x, int32_t y, int32_t z, uint64_t delay, bool ignoreBounds) {
	Vector position = {x, y, z};
	vector_add(&position, &art->position);
	if (!ignoreBounds && !_validate_position(art, &position)) {
		return false;
	}
	Vector steps;
	Vector move = {x, y, z};
	_compute_steps(&steps, &move);
	_move_steppers(art, &steps, delay);
	art->position.x += x;
	art->position.y += y;
	art->position.z += z;
	return true;
}


void home_axis(Artichoke *art) {
	Vector steps;
	move_axis_rel(art, 0, 1000, 0, PULSE_DELAY_HOMING_XY, true);
	while (!gpio_get(art->limitSwitches->z)) {
		move_axis_rel(art, 0, 0, -1, PULSE_DELAY_HOMING_Z, true);
	}
	move_axis_rel(art, 0, 0, HOME_OFFSET_Z + Z_TEMP_HOME_POSITION, PULSE_DELAY_HOMING_Z, true);
	while (!gpio_get(art->limitSwitches->x)) {
		move_axis_rel(art, -1, 0, 0, PULSE_DELAY_HOMING_XY, true);
	}
	move_axis_rel(art, HOME_OFFSET_X_Y, 0, 0, PULSE_DELAY_HOMING_XY, true);
	while (!gpio_get(art->limitSwitches->y)) {
		move_axis_rel(art, 0, -1, 0, PULSE_DELAY_HOMING_XY, true);
	}
	move_axis_rel(art, 0, HOME_OFFSET_X_Y, 0, PULSE_DELAY_HOMING_XY, true);
	move_axis_rel(art, 0, 0, -Z_TEMP_HOME_POSITION, PULSE_DELAY_HOMING_Z, true);
	art->position.x = 0;
	art->position.y = 0;
	art->position.z = 0;
}


void home_all(Artichoke *art) {
	home_axis(art);
	home_cup_holder(art);
	home_paint_dispenser(art->paintDispenser);
	home_cup_dispenser();
}


void home_cup_dispenser() {
	activate_motor(MOTOR_CUP_DISPENSER, false);
	sleep_ms(2200);
}


void eject_cup(Artichoke *art) {

}


void dispense_paint(Artichoke *art, Vector *color) {
	if (vector_equals(art->paintDispenser->color, -1, -1, -1) || !vector_comp(art->paintDispenser->color, color)) {
		eject_cup(art);
		dispense_cup(art);
	}
	// todo: set position
	home_paint_dispenser(art->paintDispenser);
	//...
}


void dispense_cup(Artichoke *art) {
	activate_motor(MOTOR_CUP_DISPENSER, true);
	sleep_ms(2200);
	activate_motor(MOTOR_CUP_DISPENSER, false);
	sleep_ms(2200);
	deactivate_motors();
}


void _wait_for_reflectance_value(bool value) {
	while (gpio_get(PIN_REFLECTANCE) != value) {
		continue;
	}
}


void set_cup_holder_position(Artichoke *art, int32_t position) {
	int32_t delta = position - art->cupHolderPosition;
	Vector v = {art->position.x, art->position.y, 0};
	artichoke_move_line(art, &v, true);
	activate_motor(MOTOR_CUP_HOLDER, delta >= 0);
	for (size_t i = 0; i < abs(delta); i++) {
		sleep_ms(1000);
		_wait_for_reflectance_value(false);
	}
	deactivate_motors();
	art->cupHolderPosition = position;
}


void home_cup_holder(Artichoke *art) {
	activate_motor(MOTOR_CUP_HOLDER, false);
	sleep_ms(6000);
	activate_motor(MOTOR_CUP_HOLDER, true);
	_wait_for_reflectance_value(false);
	deactivate_motors();
	art->cupHolderPosition = 0;
}


bool exchange_tool(Artichoke *art, uint8_t toolIndex) {
	if (toolIndex == art->toolIndex) {
		return true;
	}
	set_cup_holder_position(art, CUP_HOLDER_POSITION_HIDDEN);
	home_axis(art);
	Vector v = {0, POSITION_TOOL_CHANGER_Y_CLOSE, POSITION_TOOL_CHANGER_Z_RELEASE};
	artichoke_move_line(art, &v, true);
	// Put current tool away
	if (art->toolIndex != TOOL_INDEX_NONE) {
		v.x = _TOOL_POSITIONS[art->toolIndex];
		artichoke_move_line(art, &v, true);
		v.z = POSITION_TOOL_CHANGER_Z_GRIP;
		artichoke_move_line(art, &v, true);
		v.y = POSITION_TOOL_CHANGER_Y_HOVER;
		artichoke_move_line(art, &v, true);
		v.z = POSITION_TOOL_CHANGER_Z_RELEASE;
		artichoke_move_line(art, &v, true);
	}
	if (toolIndex == TOOL_INDEX_NONE) {
		return true;
	}
	// Pick up new tool
	v.x = _TOOL_POSITIONS[toolIndex];
	v.y = POSITION_TOOL_CHANGER_Y_HOVER;
	artichoke_move_line(art, &v, true);
	v.z = POSITION_TOOL_CHANGER_Z_GRIP;
	artichoke_move_line(art, &v, true);
	v.y = POSITION_TOOL_CHANGER_Y_CLOSE;
	artichoke_move_line(art, &v, true);
	art->toolIndex = toolIndex;
	return true;
}


void wash_tool(Artichoke *art) {
	Vector v = {art->position.x, POSITION_Y_CLEARANCE, 0};
	artichoke_move_line(art, &v, true);
	v.x = POSITION_WASHING_STATION_X;
	artichoke_move_line(art, &v, true);
	v.y = POSITION_WASHING_STATION_Y;
	artichoke_move_line(art, &v, true);
	gpio_put(PIN_ELECTROMAGNET_EN, true);
	for (size_t i = 0; i < 5; i++) {
		v.z = POSITION_WASHING_STATION_INSERT_Z;
		artichoke_move_line(art, &v, true);
		v.z = 0;
		artichoke_move_line(art, &v, true);
	}
	gpio_put(PIN_ELECTROMAGNET_EN, false);
	if (art->toolIndex != TOOL_INDEX_PAINT_MIXER && art->toolIndex != TOOL_INDEX_CUP_EXTRACTOR) {
		v.z = POSITION_WASHING_STATION_WIPE_Z;
		artichoke_move_line(art, &v, true);
		for (size_t i = 0; i < 3; i++) {
			v.x = POSITION_WASHING_STATION_WIPE_X;
			artichoke_move_line(art, &v, true);
			v.x = POSITION_WASHING_STATION_X;
			artichoke_move_line(art, &v, true);
		}
	}
	v.y = POSITION_Y_CLEARANCE;
	artichoke_move_line(art, &v, true);
}


void mix_paint(Artichoke *art) {
	exchange_tool(art, TOOL_INDEX_PAINT_MIXER);
	Vector v = {art->position.x, POSITION_Y_CLEARANCE, 0};
	artichoke_move_line(art, &v, true);
	v.x = POSITION_PAINT_MIXER_POWER_X;
	artichoke_move_line(art, &v, true);
	set_cup_holder_position(art, CUP_HOLDER_POSITION_STANDARD);
	v.z = POSITION_PAINT_MIXER_INSERTED_Z;
	artichoke_move_line(art, &v, true);
	v.y = POSITION_PAINT_MIXER_POWER_Y;
	artichoke_move_line(art, &v, true);
	sleep_ms(10000);
	v.y = POSITION_Y_CLEARANCE;
	artichoke_move_line(art, &v, true);
	v.z = 0;
	artichoke_move_line(art, &v, true);
	set_cup_holder_position(art, CUP_HOLDER_POSITION_HIDDEN);
	wash_tool(art);
}