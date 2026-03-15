# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          CNA3306)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      0)
set(PICO_GFX_GAUGE      0)
set(PICO_GFX_GRADIENT   0)
set(PICO_GFX_GRAPHICS   1)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        QSPI)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE        PIO)
# Set the SPI rate to use when communicating with the display (i.e. 40000000 for 40 MHz)
set(LCD_BAUD_RATE       125000000)
# Set the color depth of the display (1, 8, 16, or 24 bits per pixel)
set(LCD_COLOR_DEPTH     24)

# Set the width of the display in pixels
set(LCD_WIDTH           240)
# Set the height of the display in pixels
set(LCD_HEIGHT          320)
# Set the rotation of the display (0, 90, 180, or 270 degrees)
set(LCD_ROTATION        0)

# Offset in pixels from the left side of the display
set(LCD_OFFSET_X0       0)
# Offset in pixels from the right side of the display
set(LCD_OFFSET_X1       0)
# Offset in pixels from the top of the display
set(LCD_OFFSET_Y0       0)
# Offset in pixels from the bottom of the display
set(LCD_OFFSET_Y1       0) 

# Set the pin number for the pins used to connect the display to the microcontroller
set(LCD_PIN_CS          10)
set(LCD_PIN_SCL         5)
set(LCD_PIN_DAT0        6)
set(LCD_PIN_DAT1        7)
set(LCD_PIN_DAT2        8)
set(LCD_PIN_DAT3        9)
set(LCD_PIN_RST         11)

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