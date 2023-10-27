#include "handlers.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"


uint8_t homeCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	Vector steps;
	while (!gpio_get(cxy->limitSwitches->z)) {
		Vector move = {0, 0, -1};
		computeSteps(&steps, &move);
		moveSteppers(cxy, &steps, PULSE_DELAY_HOMING_Z);
	}
	while (!gpio_get(cxy->limitSwitches->x)) {
		Vector move = {-1, 0, 0};
		computeSteps(&steps, &move);
		moveSteppers(cxy, &steps, PULSE_DELAY_HOMING_XY);
	}
	while (!gpio_get(cxy->limitSwitches->y)) {
		Vector move = {0, -1, 0};
		computeSteps(&steps, &move);
		moveSteppers(cxy, &steps, PULSE_DELAY_HOMING_XY);
	}
	cxy->position.x = 0;
	cxy->position.y = 0;
	cxy->position.z = 0;
	return SUCCESS_RESPONSE;
}


uint8_t moveCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	while (i2c_get_read_available(i2c0) < 6) {
		continue;
	}
	for (size_t i = 0; i < 6; i++) {
		buffer[i + 1] = i2c_read_byte_raw(i2c0);
	}
	int32_t x = buffer[1] << 8 | buffer[2];
	int32_t y = buffer[3] << 8 | buffer[4];
	int32_t z = buffer[5] << 8 | buffer[6];
	x *= XY_MULTIPLIER;
	y *= XY_MULTIPLIER;
	z *= Z_MULTIPLIER;
	Vector position = {x, y, z};
	if (!setPosition(cxy, &position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}


uint8_t shiftCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	while (i2c_get_read_available(i2c0) < 6) {
		continue;
	}
	for (size_t i = 0; i < 6; i++) {
		buffer[i + 1] = i2c_read_byte_raw(i2c0);
	}
	Vector shift = {
		buffer[1] << 8 | buffer[2],
		buffer[3] << 8 | buffer[4],
		(buffer[5] << 8 | buffer[6]) * Z_MULTIPLIER
	};
	Vector position;
	uint32_t mask = 0b0111111111111111;
	if (shift.x >> 15 == 0) {
		position.x = cxy->position.x + shift.x & mask;
	} else {
		position.x = cxy->position.x - shift.x & mask;
	}
	if (shift.y >> 15 == 0) {
		position.y = cxy->position.y + shift.y & mask;
	} else {
		position.y = cxy->position.y - shift.y & mask;
	}
	if (shift.z >> 15 == 0) {
		position.z = cxy->position.z + shift.z & mask;
	} else {
		position.z = cxy->position.z - shift.z & mask;
	}
	if (!setPosition(cxy, &position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}