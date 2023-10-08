#include "source.h"


void setStepperDirection(Stepper *stepper, int32_t direction) {
	gpio_put(stepper->dir, direction >= 0);
}


void moveSteppers(CoreXY *cxy, Vector *steps) {
	setStepperDirection(cxy->aStepper, steps->x);
	setStepperDirection(cxy->bStepper, steps->y);
	setStepperDirection(cxy->zStepper, steps->z);
	while (steps->x > 0 || steps->y > 0 || steps->z > 0) {
		gpio_put(cxy->aStepper->stp, steps->x > 0);
		gpio_put(cxy->bStepper->stp, steps->y > 0);
		gpio_put(cxy->zStepper->stp, steps->z > 0);
		sleep_ms(STEPPER_DELAY_US);
		gpio_put(cxy->aStepper->stp, false);
		gpio_put(cxy->bStepper->stp, false);
		gpio_put(cxy->zStepper->stp, false);
		sleep_ms(STEPPER_DELAY_US);
		steps->x -= 1;
		steps->y -= 1;
		steps->z -= 1;
	}
}

bool validatePosition(CoreXY *cxy, Vector tpos) {
	if (tpos.x < 0 || tpos.x > cxy->size.x) {
		return false;
	}
	if (tpos.y < 0 || tpos.y > cxy->size.y) {
		return false;
	}
	if (tpos.z < 0 || tpos.z > cxy->size.z) {
		return false;
	}
	return true;
}

int32_t getDirection(uint32_t s, uint32_t e) {
	return e < s ? 1 : 0;
}


double vector_length(Vector v1, Vector v2) {
    int32_t dx = v1.x - v2.x;
    int32_t dy = v1.y - v2.y;
    int32_t dz = v1.z - v2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}


void add_vector(Vector *a, Vector b) {
    a->x += b.x;
    a->y += b.y;
	a->z += b.z;
}

void subtract_vector(Vector *a, Vector b) {
    a->x -= b.x;
    a->y -= b.y;
	a->z -= b.z;
}

void copy_vector(Vector *a, Vector b) {
    a->x = b.x;
    a->y = b.y;
	a->z = b.z;
}


void multiply_vector_by_scalar(Vector *v, double scalar) {
    v->x = (int32_t) round(v->x * scalar);
    v->y = (int32_t) round(v->y * scalar);
    v->z = (int32_t) round(v->z * scalar);
}


bool compare_vector(Vector v, int32_t x, int32_t y, int32_t z) {
	return v.x == x && v.y == y && v.z == z;
}


bool setPosition(CoreXY *cxy, Vector tpos) {
	/**
	 * dx(x) = 1/2 (dx(a) + dx(b))
	 * dx(y) = 1/2 (dx(a) - dx(b))
	*/
	if (!validatePosition(cxy, tpos)) {
		return false;
	}

	double length = vector_length(cxy->position, tpos);
	size_t rounded_length = (size_t) ceil(length);

	Vector prevPos;
	copy_vector(&prevPos, cxy->position);
	Vector rebasedVector;
	copy_vector(&rebasedVector, tpos);
	subtract_vector(&rebasedVector, cxy->position);
	for (size_t i = 0; i < rounded_length + 1; i++) {
		Vector delta;
		copy_vector(&delta, rebasedVector);
		multiply_vector_by_scalar(&delta, ((double) i / (double) rounded_length));
		add_vector(&delta, cxy->position);
		subtract_vector(&delta, prevPos);
		if (compare_vector(delta, 0, 0, 0)) {
			continue;
		}
		Vector steps = {
			-(delta.y + delta.x),
			delta.y - delta.x,
			delta.z
		};
		moveSteppers(cxy, &steps);
		add_vector(&prevPos, delta);
	}
	cxy->position.x = tpos.x;
	cxy->position.y = tpos.y;
	cxy->position.z = tpos.z;
	return true;
}


void pulse(uint8_t count) {
	for (uint8_t i = 0; i < count + 1; i++)
	{
		gpio_put(15, true);
	sleep_ms(20);
	gpio_put(15, false);
	sleep_ms(20);
	}
	sleep_ms(200);
}

void phel(uint8_t bit) {
	if (bit == 1) {
		gpio_put(15, true);
	} else {
		gpio_put(15, false);
	}
	sleep_ms(40);
	gpio_put(15, false);
	sleep_ms(40);
}

void show_byte(uint8_t byte) {
	gpio_put(15, true);
	sleep_ms(10);
	gpio_put(15, false);
	sleep_ms(10);
	gpio_put(15, true);
	sleep_ms(10);
	gpio_put(15, false);
	sleep_ms(10);
	phel(byte & 0b1);
	phel(byte >> 1 & 0b1);
	phel(byte >> 2 & 0b1);
	phel(byte >> 3 & 0b1);
	phel(byte >> 4 & 0b1);
	phel(byte >> 5 & 0b1);
	phel(byte >> 6 & 0b1);
	phel(byte >> 7 & 0b1);
	gpio_put(15, true);
	sleep_ms(10);
	gpio_put(15, false);
	sleep_ms(10);
	gpio_put(15, true);
	sleep_ms(10);
	gpio_put(15, false);
	sleep_ms(10);
}


void blink() {
	gpio_put(PICO_DEFAULT_LED_PIN, true);
	sleep_ms(250);
	gpio_put(PICO_DEFAULT_LED_PIN, false);
}


uint8_t homeCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	Vector position = {0, 0, 0};
	setPosition(cxy, position);
	return SUCCESS_RESPONSE;
}


uint8_t moveCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	while (i2c_get_read_available(i2c0) < 6) {
		continue;
	}
	for (size_t i = 0; i < 6; i++) {
		buffer[i + 1] = i2c_read_byte_raw(i2c0);
	}
	Vector position = {
		buffer[1] << 8 | buffer[2],
		buffer[3] << 8 | buffer[4],
		buffer[5] << 8 | buffer[6]
	};
	if (!setPosition(cxy, position)) {
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
		buffer[5] << 8 | buffer[6]
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
	if (!setPosition(cxy, position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}


void waitForCommand(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	while (true) {
		if (i2c_get_read_available(i2c0) > 0) {
			buffer[0] = i2c_read_byte_raw(i2c0);
			uint8_t code = buffer[0] >> 4;
			if (code == HOME_CODE || code == MOVE_CODE || code == SHIFT_CODE) {
				uint8_t response = ERROR_RESPONSE;
				gpio_put(BUSY_LINE, true);
				sleep_ms(25);
				if (code == HOME_CODE) {
					response = homeCommand_handler(cxy, buffer);
				} else if (code == MOVE_CODE) {
					response = moveCommand_handler(cxy, buffer);
				}
				gpio_put(BUSY_LINE, false);
				sleep_ms(25);
				gpio_put(PICO_DEFAULT_LED_PIN, true);
				while (i2c_get_read_available(i2c0) < 1) {
					continue;
				}
				i2c_read_byte_raw(i2c0);
				gpio_put(PICO_DEFAULT_LED_PIN, false);
				i2c_write_byte_raw(i2c0, response);
			}
			memset(buffer, 0, BUFFER_SIZE);
		}
	}
	
}


int main() {
	Stepper aStepper = {4, 5, 6}; // stp, dir, lim
	Stepper bStepper = {7, 8, 9};
	Stepper zStepper = {10, 11, 12};
	CoreXY cxy = {
		{20000, 20000, 16000}, // Size
		{0, 0, 0}, // Origin
		&aStepper,
		&bStepper,
		&zStepper
	};

	configure(&cxy);

	uint8_t buffer[BUFFER_SIZE];
	memset(&buffer, 0, BUFFER_SIZE);

	waitForCommand(&cxy, buffer);

	return 0;
}