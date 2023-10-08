cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(CoreXY VERSION 1.0.0)

add_executable(CoreXY)

target_sources(CoreXY PRIVATE source.c)
target_link_libraries(CoreXY PRIVATE pico_stdlib hardware_pio hardware_i2c)

pico_sdk_init()

# pico_generate_pio_header(CoreXY ${CMAKE_CURRENT_LIST_DIR}/pulse_stp_until_endstop_tripped.pio)

pico_add_extra_outputs(CoreXY)
