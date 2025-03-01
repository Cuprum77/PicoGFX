# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(PicoGFX)

# Add the library with the above sources
add_library(${PROJECT_NAME} 
    compression/compression.cpp
    compression/compression_decoder.cpp
    compression/compression_encoder.cpp
    display/display.cpp
    display/display_drivers/st7789/st7789.cpp
    display/display_drivers/gc9a01/gc9a01.cpp
    gauge/gauge.cpp
    gradient/gradient.cpp
    graphics/graphics.cpp
    graphics/bitmap.cpp
    graphics/circle.cpp
    graphics/line.cpp
    graphics/polygon.cpp
    graphics/triangle.cpp
    graphics/gfxmath.c
    graphics/trig.c
    hardware_driver/hardware_driver.cpp
    print/print.cpp
    ext/touch/touch.cpp
    ext/touch/variants/cst816/cst816.cpp
    ext/touch/variants/gt911/gt911.cpp
)

add_library(sub::Display ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/color
    PUBLIC ${PROJECT_SOURCE_DIR}/compression
    PUBLIC ${PROJECT_SOURCE_DIR}/display
    PUBLIC ${PROJECT_SOURCE_DIR}/display/display_drivers/st7789
    PUBLIC ${PROJECT_SOURCE_DIR}/display/display_drivers/gc9a01
    PUBLIC ${PROJECT_SOURCE_DIR}/gauge
    PUBLIC ${PROJECT_SOURCE_DIR}/gradient
    PUBLIC ${PROJECT_SOURCE_DIR}/graphics
    PUBLIC ${PROJECT_SOURCE_DIR}/hardware_driver
    PUBLIC ${PROJECT_SOURCE_DIR}/misc
    PUBLIC ${PROJECT_SOURCE_DIR}/print
    PUBLIC ${PROJECT_SOURCE_DIR}/print/fonts
    PUBLIC ${PROJECT_SOURCE_DIR}/ext/touch
    PUBLIC ${PROJECT_SOURCE_DIR}/ext/touch/variants/cst816
    PUBLIC ${PROJECT_SOURCE_DIR}/ext/touch/variants/gt911
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/hardware_driver/pio_spi.pio)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_dma
    hardware_pwm
    hardware_pio
    pico_multicore
)
