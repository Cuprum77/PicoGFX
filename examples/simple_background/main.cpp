#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"
#include "graphics.hpp"
#include "wee_bg.h"

// Display pins
#define DISP_PIN_DC         16
#define DISP_PIN_CS         17
#define DISP_PIN_SCK        18
#define DISP_PIN_MOSI       19
#define DISP_PIN_RST        20
#define DISP_PIN_BL         21

// Display constants
#define DISP_WIDTH          240     // 172 pixels wide
#define DISP_HEIGHT         280     // 320 p8ixels tall
#define DISP_OFFSET_X0      20		// cut off on the left side of the display
#define DISP_OFFSET_X1      20		// cut off on the right side of the display
#define DISP_OFFSET_Y0      0		// cut off on the top of the display
#define DISP_OFFSET_Y1      0		// cut off on the bottom of the display
#define DISP_ROTATION       display_rotation_t::ROTATION_90

// set the display parameters
display_spi_config_t spi_config {
	.rst = DISP_PIN_RST,
	.dc = DISP_PIN_DC,
	.cs = DISP_PIN_CS,
	.sda = DISP_PIN_MOSI,
	.scl = DISP_PIN_SCK,
	.spi_instance = spi0,
	.baudrate = 125000000,
};

display_config_t config = {
	.backlightPin = DISP_PIN_BL,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION,
	.spi = spi_config
};

// Create the display object
hardware_driver spi(&config);
st7789 display(&spi, &config);
graphics draw(display.getFrameBuffer(), &config);

int main()
{
    // Initialize the Pico C SDK
    stdio_init_all();

    // Initialize the display
    spi.init();
    display.init();
	draw.drawBitmap(background_image, 240, 280);
    display.update();

    return 0;
}
