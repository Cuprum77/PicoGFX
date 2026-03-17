# parallel8_config.cmake

# Set the driver for the display, e.g. ST7789, GC9A01, etc.
set(LCD_DRIVER          ?)
# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      ?)
set(PICO_GFX_GAUGE      ?)
set(PICO_GFX_GRADIENT   ?)
set(PICO_GFX_GRAPHICS   ?)

# Protocol used to communicate with the display
set(LCD_PROTOCOL        PARALLEL_8)
# Set the hardware interface used to communicate with the display (PIO, GPIO, or HW)
set(LCD_HARDWARE        GPIO)
# Set the color depth of the display (1, 8, 16, 18, or 24 bits per pixel)
set(LCD_COLOR_DEPTH     8)
# What memory to store the framebuffer in (SRAM or PSRAM or NONE), most use SRAM
set(LCD_FB_MEMORY       SRAM)

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
set(LCD_PIN_WR          ?)
set(LCD_PIN_RD          ?)
set(LCD_PIN_CS          ?)
set(LCD_PIN_DC          ?)
set(LCD_PIN_RST         ?)
set(LCD_PIN_IM0         ?)

# If the backlight is inverted (i.e. LOW turns on the backlight, HIGH turns off the backlight), set this to 1, otherwise set to 0
# This can be removed if no backlight pin is used
set(LCD_BACKLIGHT_INV   ?)
# If the backlight supports dimming, set this to 1, otherwise set to 0
set(LCD_BACKLIGHT_DIM   ?)
set(LCD_PIN_BL          ?)