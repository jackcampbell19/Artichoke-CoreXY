#ifndef CONSTANTS_HEADER
#define CONSTANTS_HEADER

#include <stdlib.h>
#include <stdint.h>


// VARIABLES
#define BUFFER_SIZE 16
#define I2C_ADDR 8

#define SPEED_RAMP_STEP_COUNT 2000
#define PUSLE_DELAY_MAX_US 2000
#define PULSE_DELAY_MIN_US 200
#define PUSLE_DELAY_MAX_US_Z 250
#define PULSE_DELAY_MIN_US_Z 50
#define PULSE_DELAY_HOMING_XY 300
#define PULSE_DELAY_HOMING_Z 80
#define PUSLE_DELAY_PAINT_HOR 300
#define PUSLE_DELAY_PAINT_VERT 100

#define CXY_SIZE_X 20000 // 5000
#define CXY_SIZE_Y 19240 // 4810
#define CXY_SIZE_Z 84000 // 4200

#define Z_MULTIPLIER 20
#define XY_MULTIPLIER 4

#define HOME_OFFSET_X_Y 200
#define HOME_OFFSET_Z 1000

#define PAINT_VERT_CLEAR_POSITION 0

#define SUCCESS_RESPONSE 0
#define ERROR_RESPONSE 1

#define HOME_CODE 0b0000
#define MOVE_CODE 0b0001
#define SHIFT_CODE 0b0011

#define MOTOR_CUP_DISPENSER 0
#define MOTOR_CUP_HOLDER 1
#define MOTOR_WATER_PUMP 2
#define MOTOR_OTHER 3

#define CUP_HOLDER_POSITION_HIDDEN 0
#define CUP_HOLDER_POSITION_STANDARD 1
#define CUP_HOLDER_POSITION_EXTENDED 2

// Positions
#define POSITION_Y_CLEARANCE 3000

#define POSITION_CUP_DISPENSER_X 2000
#define POSITION_CUP_DISPENSER_Y 2000

#define POSITION_TOOL_CHANGER_Z_RELEASE 2000
#define POSITION_TOOL_CHANGER_Z_GRIP 2000
#define POSITION_TOOL_CHANGER_Y_CLOSE 2000
#define POSITION_TOOL_CHANGER_Y_HOVER 2000

#define POSITION_PAINT_DISPENSER_X 2000
#define POSITION_PAINT_DISPENSER_Y 2000

#define POSITION_WASHING_STATION_X 2000
#define POSITION_WASHING_STATION_Y 2000

// PINS
#define PIN_SDA 0
#define PIN_SCL 1
#define PIN_A_DIR 2
#define PIN_A_STP 3
#define PIN_B_DIR 4
#define PIN_B_STP 5
#define PIN_AB_EN 6
#define PIN_Z_DIR 7
#define PIN_Z_STP 8
#define PIN_Z_EN 9
#define PIN_X_LIM 10
#define PIN_Y_LIM 11
#define PIN_Z_LIM 12
#define PIN_REFLECTANCE 13
#define PIN_BUSY_LINE 14
#define PIN_ELECTROMAGNET_EN 15

#define PIN_PAINT_LIM 16
#define PIN_PAINT_VERTICAL_STP 19 // temp
#define PIN_PAINT_DIR 18
#define PIN_PAINT_HORIZONTAL_STP 17 // temp
#define PIN_PAINT_EN 20
#define PIN_IN8 21
#define PIN_IN6_IN7 22
#define PIN_IN4_IN5 26
#define PIN_IN2_IN3 27
#define PIN_IN1 28

#endif
