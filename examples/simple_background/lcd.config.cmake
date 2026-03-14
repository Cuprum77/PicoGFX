# spi_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          ST7789)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      0)
set(PICO_GFX_GAUGE      0)
set(PICO_GFX_GRADIENT   0)
set(PICO_GFX_GRAPHICS   1)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        SPI)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE        HW)
# Set SPI instance to use (SPI0 or SPI1)
set(LCD_SPI_INSTANCE    SPI0)
# Set the SPI rate to use when communicating with the display (i.e. 40000000 for 40 MHz)
set(LCD_BAUD_RATE       10000000)
# Set the color depth of the display (1, 8, 16, 18, or 24 bits per pixel)
set(LCD_COLOR_DEPTH     16)

# Set the width of the display in pixels
set(LCD_WIDTH           240)
# Set the height of the display in pixels
set(LCD_HEIGHT          280)
# Set the rotation of the display (0, 90, 180, or 270 degrees)
set(LCD_ROTATION        90)

# Offset in pixels from the left side of the display
set(LCD_OFFSET_X0       0)
# Offset in pixels from the right side of the display
set(LCD_OFFSET_X1       0)
# Offset in pixels from the top of the display
set(LCD_OFFSET_Y0       20)
# Offset in pixels from the bottom of the display
set(LCD_OFFSET_Y1       20) 

# Set the pin number for the pins used to connect the display to the microcontroller
set(LCD_PIN_DC          20)
set(LCD_PIN_CS          17)
set(LCD_PIN_SCL         18)
set(LCD_PIN_SDA         19)
set(LCD_PIN_RST         16)