#include "motors.h"
#include "constants.h"
#include "pico/stdlib.h"


void activate_motor(uint32_t motorIndex, bool direction) {
	uint32_t pins[5] = {PIN_IN1, PIN_IN2_IN3, PIN_IN4_IN5, PIN_IN6_IN7, PIN_IN8};
	for (size_t i = 0; i < 5; i++) {
		gpio_put(pins[i], (i <= motorIndex) ? direction : !direction);
	}
}


void deactivate_motors() {
	uint32_t pins[5] = {PIN_IN1, PIN_IN2_IN3, PIN_IN4_IN5, PIN_IN6_IN7, PIN_IN8};
	for (size_t i = 0; i < 5; i++) {
		gpio_put(pins[i], false);
	}
}
