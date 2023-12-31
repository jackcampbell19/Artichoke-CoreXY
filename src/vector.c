#include "vector.h"
#include <math.h>


double vector_distance(Vector *v1, Vector *v2) {
    int32_t dx = v1->x - v2->x;
    int32_t dy = v1->y - v2->y;
    int32_t dz = v1->z - v2->z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}


double vector_length(Vector *v) {
    return sqrt((double)(v->x * v->x + v->y * v->y + v->z * v->z));
}


void vector_add(Vector *a, Vector *b) {
    a->x += b->x;
    a->y += b->y;
	a->z += b->z;
}


void vector_subtract(Vector *a, Vector *b) {
    a->x -= b->x;
    a->y -= b->y;
	a->z -= b->z;
}


void vector_copy(Vector *a, Vector *b) {
    a->x = b->x;
    a->y = b->y;
	a->z = b->z;
}


void vector_multiply_floor(Vector *v, double scalar) {
    v->x = (int32_t) floor(((double) v->x) * scalar);
    v->y = (int32_t) floor(((double) v->y) * scalar);
    v->z = (int32_t) floor(((double) v->z) * scalar);
}


bool vector_equals(Vector *v, int32_t x, int32_t y, int32_t z) {
	return v->x == x && v->y == y && v->z == z;
}

bool vector_comp(Vector *a, Vector *b) {
	return a->x == b->x && a->y == b->y && a->z == b->z;
}

void vector_rotate_floor(Vector *v, double degrees) {
    double radians = degrees * M_PI / 180.0;
    double cosTheta = cos(radians);
    double sinTheta = sin(radians);
    int32_t newX = round(v->x * cosTheta - v->y * sinTheta);
    int32_t newY = round(v->x * sinTheta + v->y * cosTheta);
    v->x = newX;
    v->y = newY;
}