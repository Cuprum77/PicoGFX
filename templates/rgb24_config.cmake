# rgb24_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          ?)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      0)
set(PICO_GFX_GAUGE      0)
set(PICO_GFX_GRADIENT   1)
set(PICO_GFX_GRAPHICS   1)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        PARALLEL_24)
# Set the hardware interface used to communicate with the display (PIO, GPIO, RGB, or HW)
set(LCD_HARDWARE        RGB)
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
set(LCD_PIN_DB0         ?)
set(LCD_PIN_DB1         ?)
set(LCD_PIN_DB2         ?)
set(LCD_PIN_DB3         ?)
set(LCD_PIN_DB4         ?)
set(LCD_PIN_DB5         ?)
set(LCD_PIN_DB6         ?)
set(LCD_PIN_DB7         ?)
set(LCD_PIN_DB8         ?)
set(LCD_PIN_DB9         ?)
set(LCD_PIN_DB10        ?)
set(LCD_PIN_DB11        ?)
set(LCD_PIN_DB12        ?)
set(LCD_PIN_DB13        ?)
set(LCD_PIN_DB14        ?)
set(LCD_PIN_DB15        ?)
set(LCD_PIN_DB16        ?)
set(LCD_PIN_DB17        ?)
set(LCD_PIN_DB18        ?)
set(LCD_PIN_DB19        ?)
set(LCD_PIN_DB20        ?)
set(LCD_PIN_DB21        ?)
set(LCD_PIN_DB22        ?)
set(LCD_PIN_DB23        ?)
set(LCD_PIN_DE          ?)
set(LCD_PIN_VSYNC       ?)
set(LCD_PIN_HSYNC       ?)
set(LCD_PIN_PCLK        ?)
set(LCD_PIN_RST         ?)

# If the backlight is inverted (i.e. LOW turns on the backlight, HIGH turns off the backlight), set this to 1, otherwise set to 0
# This can be removed if no backlight pin is used
set(LCD_BACKLIGHT_INV   ?)
# If the backlight supports dimming, set this to 1, otherwise set to 0
set(LCD_BACKLIGHT_DIM   ?)
set(LCD_PIN_BL          ?)