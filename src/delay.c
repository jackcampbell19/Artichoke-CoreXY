#include "delay.h"
#include <math.h>


uint64_t ease_delay_us(double current, double total, Speed *speed) {
	double half_length = total / 2;
	double threshold = total - speed->ramp;
	double x;
	if (current < half_length && current < speed->ramp) {
		// Ramping up speed
		x = current;
	} else if (current >= threshold) {
		// Ramp down speed
		x = total - current;
	} else {
		// Full speed
		return speed->min;
	}
	return ((uint64_t) (pow(x - speed->ramp, 2) / ((speed->ramp * speed->ramp) / (speed->max - speed->min))) + speed->min);
}
