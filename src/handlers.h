#ifndef HANDLERS_HEADER
#define HANDLERS_HEADER

#include "artichoke.h"
#include "constants.h"
#include <stdint.h>

uint8_t homeCommand_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint8_t moveCommand_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint8_t shiftCommand_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint8_t dispensePaint_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);
uint8_t swapTool_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]);

#endif