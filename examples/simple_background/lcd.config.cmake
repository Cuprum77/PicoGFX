# parallel8_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          ST7789)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      0)
set(PICO_GFX_GAUGE      0)
set(PICO_GFX_GRADIENT   0)
set(PICO_GFX_GRAPHICS   1)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        PARALLEL_16)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE        GPIO)
# Set the color depth of the display (1, 8, 16, 18, or 24 bits per pixel)
set(LCD_COLOR_DEPTH     16)

# Set the width of the display in pixels
set(LCD_WIDTH           240)
# Set the height of the display in pixels
set(LCD_HEIGHT          280)
# Set the rotation of the display (0, 90, 180, or 270 degrees)
set(LCD_ROTATION        0)

# Offset in pixels from the left side of the display
set(LCD_OFFSET_X0       0)
# Offset in pixels from the right side of the display
set(LCD_OFFSET_X1       0)
# Offset in pixels from the top of the display
set(LCD_OFFSET_Y0       20)
# Offset in pixels from the bottom of the display
set(LCD_OFFSET_Y1       20)

# Set the pin number for the pins used to connect the display to the microcontroller
set(LCD_PIN_DB0         15)
set(LCD_PIN_DB1         14)
set(LCD_PIN_DB2         13)
set(LCD_PIN_DB3         12)
set(LCD_PIN_DB4         11)
set(LCD_PIN_DB5         10)
set(LCD_PIN_DB6         9)
set(LCD_PIN_DB7         8)
set(LCD_PIN_DB8         7)
set(LCD_PIN_DB9         6)
set(LCD_PIN_DB10        5)
set(LCD_PIN_DB11        4)
set(LCD_PIN_DB12        3)
set(LCD_PIN_DB13        2)
set(LCD_PIN_DB14        1)
set(LCD_PIN_DB15        0)
set(LCD_PIN_WR          17)
set(LCD_PIN_RD          16)
set(LCD_PIN_CS          19)
set(LCD_PIN_DC          21)
set(LCD_PIN_RST         18)
set(LCD_PIN_IM0         20)