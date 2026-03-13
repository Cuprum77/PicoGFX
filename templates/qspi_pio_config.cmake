# spi_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER      ?)

# Protocol used to communicate with the display
set(LCD_PROTOCOL    QSPI)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE    PIO)
# Set the SPI rate to use when communicating with the display (i.e. 40000000 for 40 MHz)
set(LCD_BAUD_RATE ?)
# Set the color depth of the display (1, 8, 16, or 24 bits per pixel)
set(LCD_COLOR_DEPTH 16)


# Set the width of the display in pixels
set(LCD_WIDTH       ?)
# Set the height of the display in pixels
set(LCD_HEIGHT      ?)
# Set the rotation of the display (0, 90, 180, or 270 degrees)
set(LCD_ROTATION    ?)

# Offset in pixels from the left side of the display
set(LCD_OFFSET_X0   ?)
# Offset in pixels from the right side of the display
set(LCD_OFFSET_X1   ?)
# Offset in pixels from the top of the display
set(LCD_OFFSET_Y0   ?)
# Offset in pixels from the bottom of the display
set(LCD_OFFSET_Y1   ?) 

# Set the pin number for the pins used to connect the display to the microcontroller
set(LCD_PIN_DC      ?)
set(LCD_PIN_CS      ?)
set(LCD_PIN_SCL     ?)
set(LCD_PIN_DAT0    ?)
set(LCD_PIN_DAT1    ?)
set(LCD_PIN_DAT2    ?)
set(LCD_PIN_DAT3    ?)
set(LCD_PIN_RST     ?)