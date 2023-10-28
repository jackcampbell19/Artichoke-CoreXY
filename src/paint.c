#include "paint.h"
#include "constants.h"
#include "pico/stdlib.h"


const int32_t SYRINGE_POSITIONS[20] = {100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200};


void homePaintDispenser(PaintDispenser *pd) {
	stepperMoveRel(pd->verticalStepper, 4800, false, PUSLE_DELAY_PAINT_VERT);
	stepperMoveRel(pd->horizontalStepper, 400, false, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		stepperMoveRel(pd->horizontalStepper, 1, true, PUSLE_DELAY_PAINT_HOR);
	}
	stepperMoveRel(pd->horizontalStepper, 150, false, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		stepperMoveRel(pd->verticalStepper, 1, true, PUSLE_DELAY_PAINT_VERT);
	}
	stepperMoveRel(pd->verticalStepper, 4800, false, PUSLE_DELAY_PAINT_VERT);
	pd->horizontalStepper->current = 0;
	pd->verticalStepper->current = 0;
}


void movePaintDispenserHead(PaintDispenser *pd, int32_t syringe) {
	
}