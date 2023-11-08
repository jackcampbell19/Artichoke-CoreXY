#ifndef DELAY_HEADER
#define DELAY_HEADER


/**
 * Calculates the delay in microseconds using the default timing curve.
*/
uint64_t delay_default_us(double current, double total);


/**
 * Calculates the delay in microseconds using the fast timing curve.
*/
uint64_t delay_fast_us(double current, double total);


/**
 * Calculates the delay in microseconds using the z only timing curve.
*/
uint64_t delay_z_us(double current, double total);


#endif