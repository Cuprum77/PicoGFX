#include <stdio.h>
#include "pico/stdlib.h"
#include "uwu_bg.h"
#include "picogfx.h"

int main()
{
    stdio_init_all();
	
	draw.drawBitmap((const uint32_t *)background_image, 240, 320);
    display.update();

    uint8_t brightness = 0;
    bool increasing = true;
    while (true)
    {
        brightness += increasing ? 5 : -5;

        if (brightness == 0)
            increasing = true;
        else if (brightness == 100)
            increasing = false;

        display.setBrightness(brightness);
        sleep_ms(100);
    }

    return 0;
}
