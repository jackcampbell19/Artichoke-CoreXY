#include "constants.h"
#include "delay.h"
#include <math.h>


uint64_t _delay_us(double current, double total, double rampSteps, uint64_t maxDelay, uint64_t minDelay) {
	double half_length = total / 2;
	double threshold = total - rampSteps;
	double x;
	if (current < half_length && current < rampSteps) {
		// Ramping up speed
		x = current;
	} else if (current >= threshold) {
		// Ramp down speed
		x = total - current;
	} else {
		// Full speed
		return minDelay;
	}
	return ((uint64_t) (pow(x - rampSteps, 2) / ((rampSteps * rampSteps) / (maxDelay - minDelay))) + minDelay);
}


uint64_t delay_default_us(double current, double total) {
	return _delay_us(current, total, 1600, 1800, 200);
}


uint64_t delay_fast_us(double current, double total) {
	return _delay_us(current, total, 600, 600, 120);
}


uint64_t delay_z_us(double current, double total) {
	return _delay_us(current, total, 1600, 200, 80);
}