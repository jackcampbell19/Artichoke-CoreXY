#include "constants.h"
#include "delay.h"
#include <math.h>


const double DENOMENATOR = (SPEED_RAMP_STEP_COUNT * SPEED_RAMP_STEP_COUNT) / (PUSLE_DELAY_MAX_US - PULSE_DELAY_MIN_US);
const double DENOMENATOR_FAST = (SPEED_RAMP_STEP_COUNT * SPEED_RAMP_STEP_COUNT) / (PUSLE_DELAY_MAX_US_Z - PULSE_DELAY_MIN_US_Z);


uint64_t curved_delay_us(double x) {
	return ((uint64_t) (pow(x - SPEED_RAMP_STEP_COUNT, 2) / DENOMENATOR) + PULSE_DELAY_MIN_US);
}


uint64_t curved_delay_fast_us(double x) {
	return ((uint64_t) (pow(x - SPEED_RAMP_STEP_COUNT, 2) / DENOMENATOR_FAST) + PULSE_DELAY_MIN_US_Z);
}


uint64_t calculate_delay(double current, double total) {
	double half_length = total / 2;
	double threshold = total - SPEED_RAMP_STEP_COUNT;
	if (current < half_length && current < SPEED_RAMP_STEP_COUNT) {
		// Ramping up speed
		return curved_delay_us(current);
	} else if (current >= threshold) {
		// Ramp down speed
		return curved_delay_us(total - current);
	}
	// Full speed
	return ((uint64_t) PULSE_DELAY_MIN_US);
}


uint64_t calculate_delay_fast(double current, double total) {
	double half_length = total / 2;
	double threshold = total - SPEED_RAMP_STEP_COUNT;
	if (current < half_length && current < SPEED_RAMP_STEP_COUNT) {
		// Ramping up speed
		return curved_delay_fast_us(current);
	} else if (current >= threshold) {
		// Ramp down speed
		return curved_delay_fast_us(total - current);
	}
	// Full speed
	return ((uint64_t) PULSE_DELAY_MIN_US_Z);
}