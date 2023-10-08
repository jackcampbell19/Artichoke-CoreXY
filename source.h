#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include <math.h>

#define BUFFER_SIZE 16
#define I2C_ADDR 8
#define BUSY_LINE 14

uint32_t STEPPER_DELAY_US = 100;

uint8_t SUCCESS_RESPONSE = 0;
uint8_t ERROR_RESPONSE = 1;

uint8_t HOME_CODE = 0b0000;
uint8_t MOVE_CODE = 0b0001;
uint8_t SHIFT_CODE = 0b0011;
uint8_t NULL_CODE = 0b1111;


typedef struct {
	uint32_t stp; // Stp pin
	uint32_t dir; // Dir pin
	uint32_t lim; // Limit switch pin
} Stepper;


typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} Vector;


typedef struct {
	Vector size;
	Vector position;
	Stepper *aStepper;
	Stepper *bStepper;
	Stepper *zStepper;
} CoreXY;


double vector_length(Vector v1, Vector v2);


void configure(CoreXY *cxy) {
	stdio_init_all();

	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);

	gpio_init(BUSY_LINE);
    gpio_set_dir(BUSY_LINE, GPIO_OUT);
	gpio_put(BUSY_LINE, false);

    // Initialize I2C interface
    i2c_init(i2c0, 100000); // Initialize I2C0 with a 100kHz baud rate
    gpio_set_function(0, GPIO_FUNC_I2C); // Set GPIO2 as I2C0 SDA
    gpio_set_function(1, GPIO_FUNC_I2C); // Set GPIO3 as I2C0 SCL
	gpio_pull_up(0);
	gpio_pull_up(1);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR); // Set the device as a worker with address 7

	// Stepper A
	gpio_init(cxy->aStepper->dir);
	gpio_set_dir(cxy->aStepper->dir, GPIO_OUT);
	gpio_put(cxy->aStepper->dir, false);

	gpio_init(cxy->aStepper->stp);
	gpio_set_dir(cxy->aStepper->stp, GPIO_OUT);
	gpio_put(cxy->aStepper->stp, false);

	gpio_init(cxy->aStepper->lim);
	gpio_set_dir(cxy->aStepper->lim, GPIO_IN);
	gpio_pull_down(cxy->aStepper->lim);

	// Stepper B
	gpio_init(cxy->bStepper->dir);
	gpio_set_dir(cxy->bStepper->dir, GPIO_OUT);
	gpio_put(cxy->bStepper->dir, false);

	gpio_init(cxy->bStepper->stp);
	gpio_set_dir(cxy->bStepper->stp, GPIO_OUT);
	gpio_put(cxy->bStepper->stp, false);

	gpio_init(cxy->bStepper->lim);
	gpio_set_dir(cxy->bStepper->lim, GPIO_IN);
	gpio_pull_down(cxy->bStepper->lim);

	// Stepper Z
	gpio_init(cxy->zStepper->dir);
	gpio_set_dir(cxy->zStepper->dir, GPIO_OUT);
	gpio_put(cxy->zStepper->dir, false);

	gpio_init(cxy->zStepper->stp);
	gpio_set_dir(cxy->zStepper->stp, GPIO_OUT);
	gpio_put(cxy->zStepper->stp, false);

	gpio_init(cxy->zStepper->lim);
	gpio_set_dir(cxy->zStepper->lim, GPIO_IN);
	gpio_pull_down(cxy->zStepper->lim);
}


/**
 * Reads bytes over I2C one by one as they become available, once a
 * command btye is found, invoke the appropriate handler for the 
*/
void waitForCommand(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);

uint8_t homeCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);
uint8_t moveCommand_handler(CoreXY *cxy, uint8_t buffer[BUFFER_SIZE]);


/**
 * Validated the give position using the CoreXY.
 * @returns True if a valid position, false otherwise.
*/
bool validatePosition(CoreXY *cxy, Vector tpos);


/**
 * Sets the given steppers direction.
*/
void setStepperDirection(Stepper *stepper, int32_t direction);


/**
 * Moves the steppers in sync along the provided vector.
*/
void moveSteppers(CoreXY *cxy, Vector *steps);


/**
 * Sets the given position on the CoreXY, updates its state.
 * @returns True if the position was set successfully, false otherwise.
*/
bool setPosition(CoreXY *cxy, Vector tpos);

