#ifndef HANDLERS_HEADER
#define HANDLERS_HEADER

#include "artichoke.h"
#include "constants.h"
#include <stdint.h>

uint16_t route_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);

#endif