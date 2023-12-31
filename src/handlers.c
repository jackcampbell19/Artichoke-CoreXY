#include "handlers.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <string.h>
#include "motors.h"


uint8_t _extract_flag(uint8_t buffer[BUFFER_SIZE]) {
	return buffer[0] & 0b00001111;
}


uint8_t _extract_code(uint8_t buffer[BUFFER_SIZE]) {
    return buffer[0] >> 4;
}


/**
 * Reads numBytes bytes from I2C into buffer starting from startIndex.
*/
void _read_from_i2c(uint8_t numBytes, uint8_t startIndex, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t count = 0;
	while (count < numBytes) {
		while (i2c_get_read_available(i2c0) < 1) {
			continue;
		}
		buffer[startIndex + count] = i2c_read_byte_raw(i2c0);
		count += 1;
	}
}


uint16_t home_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t flag = _extract_flag(buffer);
	if (flag == 0b0000) {
		home_axis(art);
	} else if (flag == 0b0001) {
		// x
	} else if (flag == 0b0010) {
		// y
	} else if (flag == 0b0011) {
		// z
	} else if (flag == 0b0100) {
		// x & y
	}
	return SUCCESS_RESPONSE;
}


uint16_t move_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	_read_from_i2c(6, 1, buffer);
	uint8_t flag = _extract_flag(buffer);
	uint8_t axisFilter = (flag & 0b0110);
	bool subspace = (flag & 0b1000) >> 3 == 1;
	int32_t x = buffer[1] << 8 | buffer[2];
	int32_t y = buffer[3] << 8 | buffer[4];
	int32_t z = buffer[5] << 8 | buffer[6];
	Vector position = {x, y, z};
	if (subspace) {
		Vector tpos;
		convert_subspace_coordinate_to_position(art, &position, &tpos);
		vector_copy(&position, &tpos);
	}
	if (axisFilter == 0b0010) {
		position.y = art->position.y;
		position.z = art->position.z;
	} else if (axisFilter == 0b0100) {
		position.x = art->position.x;
		position.z = art->position.z;
	} else if (axisFilter == 0b0110) {
		position.x = art->position.x;
		position.y = art->position.y;
	}
	if (!artichoke_move_line(art, &position)) {
		return ERROR_RESPONSE;
	}
	return SUCCESS_RESPONSE;
}


uint16_t exhange_tool_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t toolIndex = _extract_flag(buffer);
	if (load_tool(art, toolIndex)) {
		return SUCCESS_RESPONSE;
	}
	return ERROR_RESPONSE;
}


uint16_t cup_position_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t flag = _extract_flag(buffer);
	uint8_t pos = flag & 0b0111;
	uint8_t moveTool = (flag & 0b1000) >> 3;
	set_cup_holder_position(art, pos, moveTool == 1);
	return SUCCESS_RESPONSE;
}


uint16_t dispense_paint_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	_read_from_i2c(COLOR_BUFFER_SIZE, 1, buffer);
	dispense_paint(art, &buffer[1]);
	return ERROR_RESPONSE;
}


uint16_t measure_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t axis = _extract_flag(buffer);
	int32_t x = axis == 0 ? -1 : 0;
	int32_t y = axis == 1 ? -1 : 0;
	int32_t z = axis == 2 ? -1 : 0;
	uint limit = axis == 0 ? art->limitSwitches->x : (axis == 1 ? art->limitSwitches->y : art->limitSwitches->z);
	uint32_t offset = axis == 2 ? HOME_OFFSET_Z : HOME_OFFSET_X_Y;
	uint16_t count = 0;
	while (!gpio_get(limit)) {
		move_axis_rel(art, x, y, z, 200, true);
		count += 1;
	}
	move_axis_rel(art, -x * count, -y * count, -z * count, 200, true);
	return count - offset;
}


uint16_t move_arc_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	_read_from_i2c(14, 1, buffer);
	uint8_t flag = _extract_flag(buffer);
	uint8_t fast = flag & 0b0111;
	uint8_t clockwise = (flag & 0b1000) >> 3;
	int32_t x0 = buffer[1] << 8 | buffer[2];
	int32_t y0 = buffer[3] << 8 | buffer[4];
	int32_t z0 = buffer[5] << 8 | buffer[6];
	int32_t x1 = buffer[7] << 8 | buffer[8];
	int32_t y1 = buffer[9] << 8 | buffer[10];
	int32_t z1 = buffer[11] << 8 | buffer[12];
	int32_t deg = buffer[13] << 8 | buffer[14];
	Vector center = {x0, y0, z0};
	Vector v = {x1, y1, z1};
	artichoke_move_arc(art, &center, &v, deg * (clockwise == 0 ? 1 : -1), fast != 0);
	return SUCCESS_RESPONSE;
}


uint16_t configure_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	_read_from_i2c(25, 1, buffer);
	uint8_t parameter = buffer[1];
	return artichoke_configure(art, buffer[1], &buffer[2]);
}


uint16_t wash_tool_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	wash_tool(art);
	return SUCCESS_RESPONSE;
}


uint16_t route_handler(Artichoke *art, uint8_t buffer[BUFFER_SIZE]) {
	uint8_t code = _extract_code(buffer);
	if (code == HOME_CODE) {
		return home_handler(art, buffer);
	} else if (code == MOVE_CODE) {
		return move_handler(art, buffer);
	} else if (code == EXCHANGE_TOOL_CODE) {
		return exhange_tool_handler(art, buffer);
	} else if (code == CUP_POSITION_CODE) {
		return cup_position_handler(art, buffer);
	} else if (code == DISPENSE_PAINT_CODE) {
		return dispense_paint_handler(art, buffer);
	} else if (code == MEASURE_CODE) {
		return measure_handler(art, buffer);
	} else if (code == MOVE_ARC_CODE) {
		return move_arc_handler(art, buffer);
	} else if (code == CONFIGURE_CODE) {
		return configure_handler(art, buffer);
	} else if (code == WASH_TOOL_CODE) {
		return wash_tool_handler(art, buffer);
	}
	// Todo: flush buffer if code invalid
	return ERROR_RESPONSE;
}