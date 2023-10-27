#ifndef HANDLERS_HEADER
#define HANDLERS_HEADER

#include "corexy.h"
#include "constants.h"
#include <stdint.h>

uint8_t homeCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);
uint8_t moveCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);
uint8_t shiftCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);

#endif