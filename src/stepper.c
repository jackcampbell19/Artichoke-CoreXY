#include "stepper.h"
#include "pico/stdlib.h"


void stepperMoveAbs(Stepper *stepper, int32_t targetStep, uint64_t delay) {
	int32_t dx = targetStep - stepper->current;
	stepperMoveRel(stepper, abs(dx), dx >= 0, delay);
}


void stepperMoveRel(Stepper *stepper, int32_t steps, bool direction, uint64_t delay) {
	gpioput(stepper->dir, direction);
	for (size_t i = 0; i < steps; i++) {
		gpio_put(stepper->stp, true);
		sleep_us(delay);
		gpio_put(stepper->stp, false);
		sleep_us(delay);
	}
	stepper->current + (direction ? steps : -steps);
}