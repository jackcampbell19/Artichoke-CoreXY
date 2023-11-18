#include "paint.h"
#include "constants.h"
#include "pico/stdlib.h"
#include "motors.h"


void home_paint_dispenser(PaintDispenser *pd) {
	move_stepper_rel(pd->horizontalStepper, 200, true, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		move_stepper_rel(pd->horizontalStepper, 1, false, PUSLE_DELAY_PAINT_HOR);
	}
	move_stepper_rel(pd->horizontalStepper, 80, true, PUSLE_DELAY_PAINT_HOR);
	while (!gpio_get(pd->limitSwitch)) {
		move_stepper_rel(pd->verticalStepper, 1, false, PUSLE_DELAY_PAINT_VERT);
	}
	move_stepper_rel(pd->verticalStepper, 200, true, PUSLE_DELAY_PAINT_VERT);
	pd->horizontalStepper->current = 0;
	pd->verticalStepper->current = 0;
}


void paint_dispenser_dispense_quantity(PaintDispenser *pd, int32_t syringe, uint8_t quantity) {
	uint32_t hover = 4000;
	uint32_t down = 2200;
	uint32_t contact = 5300;
	move_stepper_abs(pd->verticalStepper, hover, PUSLE_DELAY_PAINT_VERT);
	move_stepper_abs(pd->horizontalStepper, (syringe + 1) * 800, PUSLE_DELAY_PAINT_HOR);
	move_stepper_abs(pd->verticalStepper, contact, PUSLE_DELAY_PAINT_VERT);
	move_stepper_abs(pd->verticalStepper, PAINT_STEPS_PER_UNIT * quantity, PUSLE_DELAY_PAINT_VERT_DISPENSE);
	move_stepper_abs(pd->verticalStepper, down, PUSLE_DELAY_PAINT_VERT_DISPENSE);
	move_stepper_abs(pd->verticalStepper, hover, PUSLE_DELAY_PAINT_VERT);
}