#include "motors.h"
#include "constants.h"
#include "pico/stdlib.h"


void motorOn(uint32_t motorIndex, bool direction) {
	uint32_t pins[5] = {PIN_IN1, PIN_IN2_IN3, PIN_IN4_IN5, PIN_IN6_IN7, PIN_IN8};
	for (size_t i = 0; i < 5; i++) {
		gpio_put(pins[i], (i <= motorIndex) ? direction : !direction);
	}
}


void motorsOff() {
	uint32_t pins[5] = {PIN_IN1, PIN_IN2_IN3, PIN_IN4_IN5, PIN_IN6_IN7, PIN_IN8};
	for (size_t i = 0; i < 5; i++) {
		gpio_put(pins[i], false);
	}
}


/**
 * Dispenses a single cup from the cup dispenser.
*/
void dispenseCup() {
	motorOn(MOTOR_CUP_DISPENSER, true);
	sleep_ms(2200);
	motorOn(MOTOR_CUP_DISPENSER, false);
	sleep_ms(2200);
	motorsOff();
}


void waitForReflectanceValue(bool value) {
	while (gpio_get(PIN_REFLECTANCE) != value) {
		continue;
	}
}


/**
 * Sets the cup holder to one of the 3 positions.
*/
void setCupHolderPosition(CoreXY *cxy, int32_t position) {
	int32_t delta = cxy->cupHolderPosition - position;
	motorOn(MOTOR_CUP_HOLDER, delta >= 0);
	for (size_t i = 0; i < abs(delta); i++) {
		waitForReflectanceValue(true);
		waitForReflectanceValue(false);
	}
	motorsOff();
	cxy->cupHolderPosition = position;
}


void homeCupHolder() {
	motorOn(MOTOR_CUP_HOLDER, false);
	sleep_ms(8000);
	motorOn(MOTOR_CUP_HOLDER, true);
	waitForReflectanceValue(false);
	motorsOff();
}