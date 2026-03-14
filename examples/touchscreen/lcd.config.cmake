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
set(LCD_BAUD_RATE       40000000)
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

# Set the driver for the touch controller, e.g. CST816, GT911, etc.
set(TOUCH_DRIVER        CST816)

# Protocol used to communicate with the touch controller
set(TOUCH_PROTOCOL      I2C)
# Set I2C instance to use (I2C0 or I2C1)
set(TOUCH_I2C_INSTANCE  I2C1)
# Set the I2C speed to use when communicating with the touch controller (i.e. 400000 for 400 kHz)
set(TOUCH_I2C_SPEED     400000)

# Set the width of the touch controller in pixels
set(TOUCH_I2C_SDA_PIN   14)
set(TOUCH_I2C_SCL_PIN   15)
set(TOUCH_I2C_IRQ_PIN   13)
set(TOUCH_I2C_RST_PIN   12)

# If the backlight is inverted (i.e. LOW turns on the backlight, HIGH turns off the backlight), set this to 1, otherwise set to 0
# This can be removed if no backlight pin is used
set(LCD_BACKLIGHT_INV   0)
# If the backlight supports dimming, set this to 1, otherwise set to 0
set(LCD_BACKLIGHT_DIM   1)
set(LCD_PIN_BL          21)