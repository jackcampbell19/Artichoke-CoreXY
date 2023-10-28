#ifndef DELAY_HEADER
#define DELAY_HEADER

uint64_t curved_delay_us(double x);
uint64_t curved_delay_fast_us(double x);
uint64_t calculate_delay(double current, double total);
uint64_t calculate_delay_fast(double current, double total);

#endif