# spi_pio_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          ?)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      ?)
set(PICO_GFX_GAUGE      ?)
set(PICO_GFX_GRADIENT   ?)
set(PICO_GFX_GRAPHICS   ?)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        SPI)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE        PIO)
# Set the SPI rate to use when communicating with the display (i.e. 40000000 for 40 MHz)
set(LCD_BAUD_RATE       ?)
# Set the color depth of the display (1, 8, 16, 18, or 24 bits per pixel)
set(LCD_COLOR_DEPTH     ?)

# Set the width of the display in pixels
set(LCD_WIDTH           ?)
# Set the height of the display in pixels
set(LCD_HEIGHT          ?)
# Set the rotation of the display (0, 90, 180, or 270 degrees)
set(LCD_ROTATION        ?)

# Offset in pixels from the left side of the display
set(LCD_OFFSET_X0       ?)
# Offset in pixels from the right side of the display
set(LCD_OFFSET_X1       ?)
# Offset in pixels from the top of the display
set(LCD_OFFSET_Y0       ?)
# Offset in pixels from the bottom of the display
set(LCD_OFFSET_Y1       ?) 

# Set the pin number for the pins used to connect the display to the microcontroller
set(LCD_PIN_DC          ?)
set(LCD_PIN_CS          ?)
set(LCD_PIN_SCL         ?)
set(LCD_PIN_SDA         ?)
set(LCD_PIN_RST         ?)

# If the backlight is inverted (i.e. LOW turns on the backlight, HIGH turns off the backlight), set this to 1, otherwise set to 0
# This can be removed if no backlight pin is used
set(LCD_BACKLIGHT_INV   ?)
# If the backlight supports dimming, set this to 1, otherwise set to 0
set(LCD_BACKLIGHT_DIM   ?)
set(LCD_PIN_BL          ?)