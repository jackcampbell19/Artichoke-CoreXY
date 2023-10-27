#include "paint.h"
#include "constants.h"
#include "pico/stdlib.h"


const int32_t SYRINGE_POSITIONS[20] = {100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200, 100, 200};


void moveStepper(Stepper *stepper, uint32_t steps, size_t delay, bool direction) {
	gpio_put(stepper->dir, direction);
	for (size_t i = 0; i < steps; i++) {
		gpio_put(stepper->stp, true);
		sleep_us(delay);
		gpio_put(stepper->stp, false);
		sleep_us(delay);
	}
}


void homePaintDispenser(CoreXY *cxy) {
	// Vector cxyPosition = {0, 0, 0};
	// setPosition(cxy, &cxyPosition);
	moveStepper(cxy->paintDispenser->verticalStepper, 4800, PUSLE_DELAY_PAINT_VERT, false);
	moveStepper(cxy->paintDispenser->horizontalStepper, 400, PUSLE_DELAY_PAINT_HOR, false);
	while (!gpio_get(cxy->paintDispenser->limitSwitch)) {
		moveStepper(cxy->paintDispenser->horizontalStepper, 1, PUSLE_DELAY_PAINT_HOR, true);
	}
	moveStepper(cxy->paintDispenser->horizontalStepper, 150, PUSLE_DELAY_PAINT_HOR, false);
	while (!gpio_get(cxy->paintDispenser->limitSwitch)) {
		moveStepper(cxy->paintDispenser->verticalStepper, 1, PUSLE_DELAY_PAINT_VERT, true);
	}
	moveStepper(cxy->paintDispenser->verticalStepper, 4800, PUSLE_DELAY_PAINT_VERT, false);
	cxy->paintDispenser->position->x = 0;
	cxy->paintDispenser->position->z = 0;
}


void movePaintDispenserHead(PaintDispenser *pd, int32_t syringe) {
	int32_t dy = PAINT_VERT_CLEAR_POSITION - pd->position->y;
	moveStepper(pd->verticalStepper, abs(dy), PUSLE_DELAY_PAINT_VERT, dy >= 0);
	int32_t dx = SYRINGE_POSITIONS[syringe] - pd->position->x;
	moveStepper(pd->horizontalStepper, abs(dx), PUSLE_DELAY_PAINT_HOR, dx >= 0);
	pd->position->x = SYRINGE_POSITIONS[syringe];
}