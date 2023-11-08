#ifndef HANDLERS_HEADER
#define HANDLERS_HEADER

#include "artichoke.h"
#include "constants.h"
#include <stdint.h>

uint16_t home_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t move_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t shift_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t measure_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t dispense_paint_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t swap_tool_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint16_t subspace_move_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);

#endif