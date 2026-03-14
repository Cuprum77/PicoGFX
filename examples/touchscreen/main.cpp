#include <stdio.h>
#include "pico/stdlib.h"
#include "picogfx.h"

int main()
{
    stdio_init_all();

    // init the built in led
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    draw.fill(colors::white);
    display.update();

    uint64_t old_time = 0;
    while (1)
    {
        // blink the LED
        if (time_us_64() - old_time > 500'000)
        {
            old_time = time_us_64();
            gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        }

        if (touch.ready())
        {
            point p = point(touch.getX(), touch.getY());
            static point op = display.getCenter();

            uint32_t circle_radius = 30;
            uint32_t circle_padding = circle_radius + 1;
            uint32_t circle_thickness = 5;

            p.x = imax(p.x, circle_padding);
            p.x = imin(p.x, display.getWidth() - circle_padding);

            p.y = imax(p.y, circle_padding);
            p.y = imin(p.y, display.getHeight() - circle_padding);

            draw.drawFilledCircle(op, circle_radius + (circle_thickness * 2), colors::white);
            draw.drawFilledCircleWithStroke(p, circle_radius, colors::pink, colors::black, circle_thickness);
            display.update();
            op = p;
        }
    }
    

    return 0;
}