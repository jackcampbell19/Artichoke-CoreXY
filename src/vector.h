#ifndef VECTOR_HEADER
#define VECTOR_HEADER

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} Vector;

double vector_length(Vector *v1, Vector *v2);
void vector_add(Vector *a, Vector *b);
void vector_subtract(Vector *a, Vector *b);
void vector_copy(Vector *a, Vector *b);
void vector_multiply_floor(Vector *v, double scalar);
bool vector_equals(Vector *v, int32_t x, int32_t y, int32_t z);

#endif