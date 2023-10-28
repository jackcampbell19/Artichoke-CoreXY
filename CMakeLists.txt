cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(Artichoke VERSION 1.0.0)

add_executable(Artichoke)

file(GLOB_RECURSE SOURCES "src/*.c")

target_sources(Artichoke PRIVATE ${SOURCES})
target_link_libraries(Artichoke PRIVATE pico_stdlib hardware_pio hardware_i2c)

pico_sdk_init()

pico_add_extra_outputs(Artichoke)
