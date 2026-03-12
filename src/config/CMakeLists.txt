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

    if(DEFINED LCD_PIN_DAT0)
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT0 ${LCD_PIN_DAT0}\n")
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT1 ${LCD_PIN_DAT1}\n")
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT2 ${LCD_PIN_DAT2}\n")
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_DAT3 ${LCD_PIN_DAT3}\n")
    else()
        string(APPEND CONFIG_CONTENT "#define LCD_PIN_SDA  ${LCD_PIN_SDA}\n")

        if (DEFINED LCD_SPI_INSTANCE)
            string(APPEND CONFIG_CONTENT "#define LCD_SPI_INSTANCE ${LCD_SPI_INSTANCE}\n")
        else()
            message(FATAL_ERROR "LCD_SPI_INSTANCE must be defined for SPI protocol with HW hardware")
        endif()
        if (DEFINED LCD_SPI_RATE)
            string(APPEND CONFIG_CONTENT "#define LCD_SPI_RATE ${LCD_SPI_RATE}\n")
        else()
            message(FATAL_ERROR "LCD_SPI_RATE must be defined for SPI protocol with HW hardware")
        endif()
    endif()

else()
    message(FATAL_ERROR "Unknown LCD_PROTOCOL: ${LCD_PROTOCOL}")
endif()

# ==========================================
# Touch (optional)
# ==========================================
if(DEFINED TOUCH_DRIVER)
    string(APPEND CONFIG_CONTENT "\n#define TOUCH_DRIVER_${TOUCH_DRIVER}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_PROTOCOL_${TOUCH_PROTOCOL}\n")
    if (DEFINED TOUCH_I2C_INSTANCE)
        string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_INSTANCE ${TOUCH_I2C_INSTANCE}\n")
    endif()
    string(APPEND CONFIG_CONTENT "\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SDA_PIN ${TOUCH_I2C_SDA_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SCL_PIN ${TOUCH_I2C_SCL_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_IRQ_PIN ${TOUCH_I2C_IRQ_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_RST_PIN ${TOUCH_I2C_RST_PIN}\n")
    string(APPEND CONFIG_CONTENT "#define TOUCH_I2C_SPEED   ${TOUCH_I2C_SPEED}\n")
endif()

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lcd_config.h CONTENT "${CONFIG_CONTENT}")

add_library(lcd_config INTERFACE)
target_include_directories(lcd_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR})