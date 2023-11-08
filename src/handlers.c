#include "handlers.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"


uint16_t home_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	home_axis(art);
	return SUCCESS_RESPONSE;
}


uint16_t move_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
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
	if (!artichoke_set_position(art, &position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}


uint16_t shift_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
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
		position.x = art->position.x + shift.x & mask;
	} else {
		position.x = art->position.x - shift.x & mask;
	}
	if (shift.y >> 15 == 0) {
		position.y = art->position.y + shift.y & mask;
	} else {
		position.y = art->position.y - shift.y & mask;
	}
	if (shift.z >> 15 == 0) {
		position.z = art->position.z + shift.z & mask;
	} else {
		position.z = art->position.z - shift.z & mask;
	}
	if (!artichoke_set_position(art, &position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}


uint16_t measure_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	while (i2c_get_read_available(i2c0) < 1) {
		continue;
	}
	uint8_t axis = i2c_read_byte_raw(i2c0);
	int32_t x = axis == 0 ? -1 : 0;
	int32_t y = axis == 1 ? -1 : 0;
	int32_t z = axis == 2 ? -1 : 0;
	uint32_t offset = axis == 2 ? HOME_OFFSET_Z : HOME_OFFSET_X_Y;
	uint16_t count = 0;
	while (!gpio_get(art->limitSwitches->z)) {
		move_axis_rel(art, x, y, z, PULSE_DELAY_HOMING_XY, true);
		count += 1;
	}
	move_axis_rel(art, -x * count, -y * count, -z * count, PULSE_DELAY_HOMING_XY, true);
	return count - offset;
}