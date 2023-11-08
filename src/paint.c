#include "paint.h"
#include "constants.h"
#include "pico/stdlib.h"


const int32_t SYRINGE_POSITIONS[20] = {100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200};


void home_paint_dispenser(PaintDispenser *pd) {
	move_stepper_rel(pd->verticalStepper, 4800, false, PUSLE_DELAY_PAINT_VERT);
	move_stepper_rel(pd->horizontalStepper, 400, false, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		move_stepper_rel(pd->horizontalStepper, 1, true, PUSLE_DELAY_PAINT_HOR);
	}
	move_stepper_rel(pd->horizontalStepper, 150, false, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		move_stepper_rel(pd->verticalStepper, 1, true, PUSLE_DELAY_PAINT_VERT);
	}
	move_stepper_rel(pd->verticalStepper, 4800, false, PUSLE_DELAY_PAINT_VERT);
	pd->horizontalStepper->current = 0;
	pd->verticalStepper->current = 0;
}


void move_paint_dispenser_head(PaintDispenser *pd, int32_t syringe) {
	
}