#include "source.h"
#include "constants.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "handlers.h"
#include <string.h>
#include "artichoke.h"
#include "paint.h"
#include "motors.h"


void init_out(uint32_t pin) {
	gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
	gpio_put(pin, false);
}


void init_stepper(Stepper *stepper, bool initEn) {
	gpio_init(stepper->dir);
	gpio_set_dir(stepper->dir, GPIO_OUT);
	gpio_put(stepper->dir, false);
	gpio_init(stepper->stp);
	gpio_set_dir(stepper->stp, GPIO_OUT);
	gpio_put(stepper->stp, false);
	if (initEn) {
		init_out(stepper->en);
	}
}


void init_in(uint32_t pin) {
	gpio_init(pin);
	gpio_set_dir(pin, GPIO_IN);
	gpio_pull_down(pin);
}


/**
 * Configures the board for the program.
*/
void configure(Artichoke *art) {
	stdio_init_all();
	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	// Busy line
	init_out(PIN_BUSY_LINE);
    // Initialize I2C interface
    i2c_init(i2c0, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SDA);
	gpio_pull_up(PIN_SCL);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);
	// Stepper A
	init_stepper(art->aStepper, true);
	init_in(art->limitSwitches->x);
	// Stepper B
	init_stepper(art->bStepper, false);
	init_in(art->limitSwitches->y);
	// Stepper Z
	init_stepper(art->zStepper, true);
	init_in(art->limitSwitches->z);
	// Paint Dispenser
	init_out(art->paintDispenser->electromagnet);
	init_in(art->paintDispenser->limitSwitch);
	init_stepper(art->paintDispenser->horizontalStepper, true);
	init_stepper(art->paintDispenser->verticalStepper, false);
	// Motors
	init_out(PIN_IN1);
	init_out(PIN_IN2_IN3);
	init_out(PIN_IN4_IN5);
	init_out(PIN_IN6_IN7);
	init_out(PIN_IN8);
}


/**
 * Waits for a command to be issued over I2C and proccesses it once it is
 * recieved. Responds over I2C with response code.
*/
void wait_for_command(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	while (true) {
		if (i2c_get_read_available(i2c0) > 0) {
			buffer[0] = i2c_read_byte_raw(i2c0);
			uint8_t code = buffer[0] >> 4;
			if (code == HOME_CODE || code == MOVE_CODE || code == SHIFT_CODE) {
				uint16_t response = ERROR_RESPONSE;
				gpio_put(PIN_BUSY_LINE, true);
				sleep_ms(25);
				if (code == HOME_CODE) {
					response = home_handler(art, buffer);
				} else if (code == MOVE_CODE) {
					response = move_handler(art, buffer);
				} else if (code == MEASURE_CODE) {
					response = measure_handler(art, buffer);
				}
				gpio_put(PIN_BUSY_LINE, false);
				sleep_ms(25);
				gpio_put(PICO_DEFAULT_LED_PIN, true);
				while (i2c_get_read_available(i2c0) < 1) {
					continue;
				}
				i2c_read_byte_raw(i2c0);
				gpio_put(PICO_DEFAULT_LED_PIN, false);
				i2c_write_byte_raw(i2c0, response & 0b11111111);
				i2c_write_byte_raw(i2c0, (response >> 8) & 0b11111111);
			}
			memset(buffer, 0, BUFFER_SIZE);
		}
	}
}


void testAll(Artichoke *art) {
	home_axis(art);
	home_cup_holder(art);
	home_cup_dispenser(art);
	set_cup_holder_position(art, CUP_HOLDER_POSITION_EXTENDED);
	dispense_cup(art);
	sleep_ms(1000);
	set_cup_holder_position(art, CUP_HOLDER_POSITION_HIDDEN);
}


int main() {
	// Stepper motors
	Stepper aStepper = {PIN_A_STP, PIN_A_DIR, PIN_AB_EN, 0};
	Stepper bStepper = {PIN_B_STP, PIN_B_DIR, PIN_AB_EN, 0};
	Stepper zStepper = {PIN_Z_STP, PIN_Z_DIR, PIN_Z_EN, 0};
	Stepper paintHorizontalStepper = {PIN_PAINT_HORIZONTAL_STP, PIN_PAINT_DIR, PIN_PAINT_EN, 0};
	Stepper paintVerticalStepper = {PIN_PAINT_VERTICAL_STP, PIN_PAINT_DIR, PIN_PAINT_EN, 0};
	// Limit switches
	Vector limitSwitches = {PIN_X_LIM, PIN_Y_LIM, PIN_Z_LIM};
	// Paint dispenser
	Vector color = {-1, -1, -1};
	PaintDispenser paintDispenser = {
		&paintHorizontalStepper,
		&paintVerticalStepper,
		PIN_PAINT_LIM,
		PIN_ELECTROMAGNET_EN,
		&color
	};
	// Artichoke
	Artichoke art = {
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
	configure(&art);
	// artichokeHomeAll(&art);
	uint8_t buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	// waitForCommand(&art, buffer);
	while (true) {
		testAll(&art);
	}
	return 0;
}