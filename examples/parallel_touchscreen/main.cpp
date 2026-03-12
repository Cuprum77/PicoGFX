#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"
#include "graphics.hpp"

// Display pins
#define DISP_DB0    15
#define DISP_DB1    14
#define DISP_DB2    13
#define DISP_DB3    12
#define DISP_DB4    11
#define DISP_DB5    10
#define DISP_DB6    9
#define DISP_DB7    8
#define DISP_DB8    7
#define DISP_DB9    6
#define DISP_DB10   5
#define DISP_DB11   4
#define DISP_DB12   3
#define DISP_DB13   2
#define DISP_DB14   1
#define DISP_DB15   0
#define DISP_RD     16
#define DISP_WR     17
#define DISP_RSTB   18
#define DISP_CS     19
#define DISP_IM0    20
#define DISP_RS     21  // Also known as DC

// Display constants
#define DISP_WIDTH          240     // 172 pixels wide
#define DISP_HEIGHT         280     // 320 pixels tall
#define DISP_OFFSET_X0      0		// cut off on the left side of the display
#define DISP_OFFSET_X1      0		// cut off on the right side of the display
#define DISP_OFFSET_Y0      20		// cut off on the top of the display
#define DISP_OFFSET_Y1      20		// cut off on the bottom of the display
#define DISP_ROTATION       display_rotation_t::ROTATION_0

// set the display parameters
display_8080_config_t display_config {
    .db = {DISP_DB0, DISP_DB1, DISP_DB2, DISP_DB3, DISP_DB4, DISP_DB5, DISP_DB6, DISP_DB7,
            DISP_DB8, DISP_DB9, DISP_DB10, DISP_DB11, DISP_DB12, DISP_DB13, DISP_DB14, DISP_DB15},
    .db_size = 16,
    .rst = DISP_RSTB,
    .csx = DISP_CS,
    .dcx = DISP_RS,
    .rdx = DISP_RD,
    .wrx = DISP_WR,
    .im0 = DISP_IM0,
};

display_config_t config = {
	.dimming = false,
    .inverseColors = true,
	.height = DISP_HEIGHT,
	.width = DISP_WIDTH,
	.columnOffset1 = DISP_OFFSET_X0,
	.columnOffset2 = DISP_OFFSET_X1,
	.rowOffset1 = DISP_OFFSET_Y0,
	.rowOffset2 = DISP_OFFSET_Y1,
	.rotation = DISP_ROTATION,
    .interface = display_interface_t::DISPLAY_8080,
    .b8080 = display_config,
};

hardware_driver hw(&config);
st7789 display(&hw, &config);
graphics draw(display.getFrameBuffer(), &config);

int main()
{
    // Initialize the Pico C SDK
    stdio_init_all();

    // Initialize the display
    hw.init();
    display.init();
    draw.fill(colors::white);
    draw.testPattern();
    display.update();

    return 0;
}
