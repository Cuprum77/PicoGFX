# lcd/config/CMakeLists.txt
include(${CMAKE_SOURCE_DIR}/lcd.config.cmake)

set(CONFIG_CONTENT "/* Auto-generated */\n\n")

# ==========================================
# Display geometry
# ==========================================
string(APPEND CONFIG_CONTENT "#define LCD_WIDTH       ${LCD_WIDTH}\n")
string(APPEND CONFIG_CONTENT "#define LCD_HEIGHT      ${LCD_HEIGHT}\n\n")
string(APPEND CONFIG_CONTENT "#define LCD_OFFSET_X0   ${LCD_OFFSET_X0}\n")
string(APPEND CONFIG_CONTENT "#define LCD_OFFSET_X1   ${LCD_OFFSET_X1}\n")
string(APPEND CONFIG_CONTENT "#define LCD_OFFSET_Y0   ${LCD_OFFSET_Y0}\n")
string(APPEND CONFIG_CONTENT "#define LCD_OFFSET_Y1   ${LCD_OFFSET_Y1}\n")
string(APPEND CONFIG_CONTENT "#define LCD_ROTATION_${LCD_ROTATION}\n\n")

# ==========================================
# Protocol
# ==========================================
string(APPEND CONFIG_CONTENT "#define LCD_DRIVER_${LCD_DRIVER}\n")
string(APPEND CONFIG_CONTENT "#define LCD_HARDWARE_${LCD_HARDWARE}\n")
string(APPEND CONFIG_CONTENT "#define LCD_COLOR_DEPTH_${LCD_COLOR_DEPTH}\n")
string(APPEND CONFIG_CONTENT "#define LCD_COLOR_DEPTH ${LCD_COLOR_DEPTH}\n")
string(APPEND CONFIG_CONTENT "#define LCD_PROTOCOL_${LCD_PROTOCOL}\n\n")

if(LCD_PROTOCOL STREQUAL "PARALLEL_8" OR
   LCD_PROTOCOL STREQUAL "PARALLEL_16" OR
   LCD_PROTOCOL STREQUAL "PARALLEL_24")

    if(LCD_PROTOCOL STREQUAL "PARALLEL_8")
        set(DB_COUNT 8)
    elseif(LCD_PROTOCOL STREQUAL "PARALLEL_16")
        set(DB_COUNT 16)
    elseif(LCD_PROTOCOL STREQUAL "PARALLEL_24")
        set(DB_COUNT 24)
    endif()

    math(EXPR DB_MAX "${DB_COUNT} - 1")
    foreach(i RANGE 0 ${DB_MAX})
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DB${i}\t${LCD_PIN_DB${i}}\n")
    endforeach()

    string(APPEND CONFIG_CONTENT "\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_WR  ${LCD_PIN_WR}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_RD  ${LCD_PIN_RD}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_CS  ${LCD_PIN_CS}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DC  ${LCD_PIN_DC}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_RST ${LCD_PIN_RST}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_IM0 ${LCD_PIN_IM0}\n")

    # After the DB pin defines are written, check ordering
    set(DB_PINS "")
    foreach(i RANGE 0 ${DB_MAX})
        list(APPEND DB_PINS ${LCD_PIN_DB${i}})
    endforeach()

    # Check ascending sequential
    set(IS_SEQUENTIAL TRUE)
    set(PREV_PIN -1)
    foreach(pin ${DB_PINS})
        if(NOT PREV_PIN EQUAL -1)
            math(EXPR EXPECTED "${PREV_PIN} + 1")
            if(NOT pin EQUAL EXPECTED)
                set(IS_SEQUENTIAL FALSE)
            endif()
        endif()
        set(PREV_PIN ${pin})
    endforeach()

    # Check descending sequential
    set(IS_REVERSED TRUE)
    set(PREV_PIN -1)
    foreach(pin ${DB_PINS})
        if(NOT PREV_PIN EQUAL -1)
            math(EXPR EXPECTED "${PREV_PIN} - 1")
            if(NOT pin EQUAL EXPECTED)
                set(IS_REVERSED FALSE)
            endif()
        endif()
        set(PREV_PIN ${pin})
    endforeach()

    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DB_COUNT   ${DB_COUNT}\n")

    if(IS_SEQUENTIAL)
        list(GET DB_PINS 0 DB_BASE_PIN)
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DB_SEQUENTIAL\n")
    elseif(IS_REVERSED)
        list(GET DB_PINS 0 DB_BASE_PIN)
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DB_SEQUENTIAL\n")
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DB_REVERSED\n")
        string(APPEND CONFIG_CONTENT "#define LCD_INVERT_COLORS\n\n")
    endif()

    set(DB_ARRAY_CONTENT "")
    if(IS_REVERSED)
        list(REVERSE DB_PINS)
    endif()

    set(DB_ARRAY_CONTENT "")
    foreach(i RANGE 0 ${DB_MAX})
        list(APPEND DB_ARRAY_CONTENT ${LCD_PIN_DB${i}})
    endforeach()

    string(JOIN ", " DB_ARRAY_CONTENT ${DB_ARRAY_CONTENT})
    string(APPEND CONFIG_CONTENT "\n#define LCD_PIN_DB_ARRAY { ${DB_ARRAY_CONTENT} }\n")

elseif(LCD_PROTOCOL STREQUAL "SPI")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_SCL  ${LCD_PIN_SCL}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_CS   ${LCD_PIN_CS}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DC   ${LCD_PIN_DC}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_RST  ${LCD_PIN_RST}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_SDA  ${LCD_PIN_SDA}\n")

    if (DEFINED LCD_SPI_INSTANCE AND LCD_HARDWARE STREQUAL "HW")
        string(APPEND CONFIG_CONTENT "#define LCD_SPI_INSTANCE_${LCD_SPI_INSTANCE}\n")
    elseif (LCD_HARDWARE STREQUAL "HW" AND NOT DEFINED LCD_SPI_INSTANCE)
        message(FATAL_ERROR "LCD_SPI_INSTANCE must be defined for SPI protocol with HW hardware")
    endif()
    if (DEFINED LCD_BAUD_RATE)
        string(APPEND CONFIG_CONTENT "#define LCD_BAUD_RATE ${LCD_BAUD_RATE}\n")
    else()
        message(FATAL_ERROR "LCD_BAUD_RATE must be defined for SPI protocol with HW hardware")
    endif()

elseif(LCD_PROTOCOL STREQUAL "QSPI")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_SCL  ${LCD_PIN_SCL}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_CS   ${LCD_PIN_CS}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DC   ${LCD_PIN_DC}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_RST  ${LCD_PIN_RST}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT0 ${LCD_PIN_DAT0}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT1 ${LCD_PIN_DAT1}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT2 ${LCD_PIN_DAT2}\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT3 ${LCD_PIN_DAT3}\n")

    # Check if they are sequential
    set(DAT_PINS "${LCD_PIN_DAT0}, ${LCD_PIN_DAT1}, ${LCD_PIN_DAT2}, ${LCD_PIN_DAT3}")
    set(DAT_PIN_LIST ${LCD_PIN_DAT0} ${LCD_PIN_DAT1} ${LCD_PIN_DAT2} ${LCD_PIN_DAT3})
    set(IS_SEQUENTIAL TRUE)
    set(PREV_PIN -1)
    foreach(pin ${DAT_PIN_LIST})
        if(NOT PREV_PIN EQUAL -1)
            math(EXPR EXPECTED "${PREV_PIN} + 1")
            if(NOT pin EQUAL EXPECTED)
                set(IS_SEQUENTIAL FALSE)
            endif()
        endif()
        set(PREV_PIN ${pin})
    endforeach()

    # Check if they are reversed sequential
    set(IS_REVERSED TRUE)
    set(PREV_PIN -1)
    foreach(pin ${DAT_PIN_LIST})
        if(NOT PREV_PIN EQUAL -1)
            math(EXPR EXPECTED "${PREV_PIN} - 1")
            if(NOT pin EQUAL EXPECTED)
                set(IS_REVERSED FALSE)
            endif()
        endif()
        set(PREV_PIN ${pin})
    endforeach()

    string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT_COUNT 4\n")
    if(IS_SEQUENTIAL)
        list(GET DAT_PIN_LIST 0 DAT_BASE_PIN)
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT_SEQUENTIAL\n")
    elseif(IS_REVERSED)
        list(GET DAT_PIN_LIST 0 DAT_BASE_PIN)
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT_SEQUENTIAL\n")
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT_REVERSED\n")
        string(APPEND CONFIG_CONTENT "#define LCD_INVERT_COLORS\n\n")
    endif()
    
    if (DEFINED LCD_BAUD_RATE)
        string(APPEND CONFIG_CONTENT "#define LCD_BAUD_RATE ${LCD_BAUD_RATE}\n")
    else()
        message(FATAL_ERROR "LCD_BAUD_RATE must be defined for SPI protocol with HW hardware")
    endif()
else()
    message(FATAL_ERROR "Unknown LCD_PROTOCOL: ${LCD_PROTOCOL}")
endif()

# ==========================================
# Backlight (optional)
# ==========================================

if (DEFINED LCD_PIN_BL)
    string(APPEND CONFIG_CONTENT "\n#define LCD_BACKLIGHT_ENABLED\n")
    string(APPEND CONFIG_CONTENT "#define LCD_PIN_BL ${LCD_PIN_BL}\n")
    if(LCD_BACKLIGHT_INV)
        string(APPEND CONFIG_CONTENT "#define LCD_BACKLIGHT_INVERTED\n")
    endif()
    if(LCD_BACKLIGHT_DIM)
        string(APPEND CONFIG_CONTENT "#define LCD_BACKLIGHT_DIMMABLE\n")
    endif()
endif()

# ==========================================
# Touch (optional)
# ==========================================
if(DEFINED TOUCH_DRIVER)
    string(APPEND CONFIG_CONTENT "\n/* Touch configuration */\n\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_ENABLED\n")
    string(APPEND CONFIG_CONTENT "\n#define TOUCH_DRIVER_${TOUCH_DRIVER}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_PROTOCOL_${TOUCH_PROTOCOL}\n")
    if (DEFINED TOUCH_I2C_INSTANCE)
        string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_INSTANCE_${TOUCH_I2C_INSTANCE}\n")
    endif()
    string(APPEND CONFIG_CONTENT "\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SDA_PIN ${TOUCH_I2C_SDA_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SCL_PIN ${TOUCH_I2C_SCL_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_IRQ_PIN ${TOUCH_I2C_IRQ_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_RST_PIN ${TOUCH_I2C_RST_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SPEED   ${TOUCH_I2C_SPEED}\n")
endif()

# ==========================================
# PicoGFX configuration
# ==========================================
if(PICO_GFX_PRINT)
    if(NOT PICO_GFX_FONTS)
        message(WARNING "PICO_GFX_PRINT is enabled but no fonts are specified, disabling print")
    else()
        string(APPEND CONFIG_CONTENT "#define PICO_GFX_PRINT\n")
    endif()
endif()
if(PICO_GFX_GAUGE)
    string(APPEND CONFIG_CONTENT "#define PICO_GFX_GAUGE\n")
endif()

if(PICO_GFX_GRADIENT)
    string(APPEND CONFIG_CONTENT "#define PICO_GFX_GRADIENT\n")
endif()

if(PICO_GFX_GRAPHICS)
    string(APPEND CONFIG_CONTENT "#define PICO_GFX_GRAPHICS\n")
endif()

# If no touch driver is defined, set TOUCH_ENABLED to 0
if(NOT DEFINED TOUCH_DRIVER)
    string(APPEND CONFIG_CONTENT "#define TOUCH_DISABLED\n")
endif()

# ==========================================
# Font generation
# ==========================================

if(PICO_GFX_PRINT)
    find_package(Python3 REQUIRED)

    set(PICO_GFX_FONT_CACHE ${CMAKE_BINARY_DIR}/fonts)

    add_custom_target(generate_fonts
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PICO_GFX_FONT_CACHE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/print/fonts/
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/download_fonts.py
                ${PICO_GFX_FONTS}
                --output ${PICO_GFX_FONT_CACHE}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate_fonts.py
                ${PICO_GFX_FONTS}
                --font-dir ${PICO_GFX_FONT_CACHE}
                --sizes ${PICO_GFX_FONT_SIZES}
                --output ${CMAKE_BINARY_DIR}/src/print/fonts/
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
        COMMENT "Downloading and generating fonts..."
    )

    set(FONTS_INCLUDE_CONTENT "#pragma once\n\n#include \"fontstruct.h\"\n\n")
    foreach(font ${PICO_GFX_FONTS})
        string(REPLACE ".ttf" "" font_name ${font})
        foreach(size ${PICO_GFX_FONT_SIZES})
            string(APPEND FONTS_INCLUDE_CONTENT "#include \"${font_name}${size}.font\"\n")
        endforeach()
    endforeach()

    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/src/print/fonts/pico_gfx_fonts.h CONTENT "${FONTS_INCLUDE_CONTENT}")
endif()

# ==========================================
# Generate lcd_config.h
# ==========================================

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lcd_config.h CONTENT "${CONFIG_CONTENT}")

add_library(lcd_config INTERFACE)
target_include_directories(lcd_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR})