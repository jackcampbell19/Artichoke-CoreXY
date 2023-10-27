#include "source.h"
#include "constants.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "handlers.h"
#include <string.h>
#include "corexy.h"
#include "paint.h"
#include "motors.h"


void initOut(uint32_t pin) {
	gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
	gpio_put(pin, false);
}


void initStepper(Stepper *stepper, bool initEn) {
	gpio_init(stepper->dir);
	gpio_set_dir(stepper->dir, GPIO_OUT);
	gpio_put(stepper->dir, false);
	gpio_init(stepper->stp);
	gpio_set_dir(stepper->stp, GPIO_OUT);
	gpio_put(stepper->stp, false);
	if (initEn) {
		initOut(stepper->en);
	}
}


void initIn(uint32_t pin) {
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_IN);
	gpio_pull_down(pin);
}


/**
 * Configures the board for the program.
*/
void configure(CoreXY *cxy) {
	stdio_init_all();
	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	// Busy line
	initOut(PIN_BUSY_LINE);
    // Initialize I2C interface
    i2c_init(i2c0, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SDA);
	gpio_pull_up(PIN_SCL);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);
	// Stepper A
	initStepper(cxy->aStepper, true);
	initIn(cxy->limitSwitches->x);
	// Stepper B
	initStepper(cxy->bStepper, false);
	initIn(cxy->limitSwitches->y);
	// Stepper Z
	initStepper(cxy->zStepper, true);
	initIn(cxy->limitSwitches->z);
	// Paint Dispenser
	initOut(cxy->paintDispenser->electromagnet);
	initIn(cxy->paintDispenser->limitSwitch);
	initStepper(cxy->paintDispenser->horizontalStepper, true);
	initStepper(cxy->paintDispenser->verticalStepper, false);
	// Motors
	initOut(PIN_IN1);
	initOut(PIN_IN2_IN3);
	initOut(PIN_IN4_IN5);
	initOut(PIN_IN6_IN7);
	initOut(PIN_IN8);
}


/**
 * Waits for a command to be issues over I2C and proccessed it once it is
 * recieved.
*/
void waitForCommand(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]) {
	while (true) {
		if (i2c_get_read_available(i2c0) > 0) {
			buffer[0] = i2c_read_byte_raw(i2c0);
			uint8_t code = buffer[0] >> 4;
			if (code == HOME_CODE || code == MOVE_CODE || code == SHIFT_CODE) {
				uint8_t response = ERROR_RESPONSE;
				gpio_put(PIN_BUSY_LINE, true);
				sleep_ms(25);
				if (code == HOME_CODE) {
					response = homeCommand_handler(cxy, buffer);
				} else if (code == MOVE_CODE) {
					response = moveCommand_handler(cxy, buffer);
				}
				gpio_put(PIN_BUSY_LINE, false);
				sleep_ms(25);
				gpio_put(PICO_DEFAULT_LED_PIN, true);
				while (i2c_get_read_available(i2c0) < 1) {
					continue;
				}
				i2c_read_byte_raw(i2c0);
				gpio_put(PICO_DEFAULT_LED_PIN, false);
				i2c_write_byte_raw(i2c0, response);
			}
			memset(buffer, 0, BUFFER_SIZE);
		}
	}
}


int main() {
	// Stepper motors
	Stepper aStepper = {PIN_A_STP, PIN_A_DIR, PIN_AB_EN};
	Stepper bStepper = {PIN_B_STP, PIN_B_DIR, PIN_AB_EN};
	Stepper zStepper = {PIN_Z_STP, PIN_Z_DIR, PIN_Z_EN};
	Stepper paintHorizontalStepper = {PIN_PAINT_HORIZONTAL_STP, PIN_PAINT_DIR, PIN_PAINT_EN};
	Stepper paintVerticalStepper = {PIN_PAINT_VERTICAL_STP, PIN_PAINT_DIR, PIN_PAINT_EN};
	// Limit switches
	Vector limitSwitches = {PIN_X_LIM, PIN_Y_LIM, PIN_Z_LIM};
	// Position
	Vector paintPosition = {0, 0, 0};
	// Paint dispenser
	PaintDispenser paintDispenser = {
		&paintHorizontalStepper,
		&paintVerticalStepper,
		PIN_PAINT_LIM,
		PIN_ELECTROMAGNET_EN,
		&paintPosition
	};
	// CoreXY
	CoreXY cxy = {
		{CXY_SIZE_X, CXY_SIZE_Y, CXY_SIZE_Z},
		{0, 0, 0},
		&aStepper,
		&bStepper,
		&zStepper,
		&limitSwitches,
		&paintDispenser,
		CUP_HOLDER_POSITION_HIDDEN
	};
	// Configure pins
	configure(&cxy);
	uint8_t buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	// waitForCommand(&cxy, buffer);
	// homePaintDispenser(&cxy);
	homeCupHolder();
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_STANDARD);
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_HIDDEN);
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_EXTENDED);
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_STANDARD);
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_HIDDEN);
	setCupHolderPosition(&cxy, CUP_HOLDER_POSITION_EXTENDED);
	return 0;
}