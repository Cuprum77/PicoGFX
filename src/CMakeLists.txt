# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(PicoGFX)
add_subdirectory(config)

include(${CMAKE_SOURCE_DIR}/lcd.config.cmake)

# Add the library with the above sources
add_library(${PROJECT_NAME} 
    compression/compression.cpp
    compression/compression_decoder.cpp
    compression/compression_encoder.cpp
    display/backlight.cpp
    display/display.cpp
    drivers/driver.cpp
    drivers/cna3306/cna3306.cpp
    drivers/st7789/st7789.cpp
    drivers/gc9a01/gc9a01.cpp
    drivers/touch/touch.cpp
    drivers/touch/cst816/cst816.cpp
    drivers/touch/gt911/gt911.cpp
    gauge/gauge.cpp
    gradient/gradient.cpp
    graphics/graphics.cpp
    graphics/bitmap.cpp
    graphics/circle.cpp
    graphics/line.cpp
    graphics/polygon.cpp
    graphics/triangle.cpp
    graphics/filter.cpp
    graphics/gfxmath.c
    graphics/trig.c
    print/print.cpp
    picogfx.cpp
)

add_library(sub::Display ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/color
    PUBLIC ${PROJECT_SOURCE_DIR}/compression
    PUBLIC ${PROJECT_SOURCE_DIR}/display
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/cna3306
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/st7789
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/gc9a01
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/touch
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/touch/cst816
    PUBLIC ${PROJECT_SOURCE_DIR}/drivers/touch/gt911
    PUBLIC ${PROJECT_SOURCE_DIR}/gauge
    PUBLIC ${PROJECT_SOURCE_DIR}/gradient
    PUBLIC ${PROJECT_SOURCE_DIR}/graphics
    PUBLIC ${PROJECT_SOURCE_DIR}/shapes
    PUBLIC ${PROJECT_SOURCE_DIR}/print
    PUBLIC ${CMAKE_BINARY_DIR}/src/print/fonts
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/drivers/pio_spi.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/drivers/pio_qspi.pio)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_dma
    hardware_pwm
    hardware_pio
    pico_multicore
    lcd_config
)

if(PICO_GFX_PRINT)
    add_dependencies(${PROJECT_NAME} generate_fonts)
endif()

target_link_options(${PROJECT_NAME} INTERFACE -Wl,--wrap=stdio_init_all)
target_compile_options(${PROJECT_NAME} INTERFACE -include picogfx.h)