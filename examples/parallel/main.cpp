#include <stdio.h>
#include "pico/stdlib.h"
#include "picogfx.h"

int main()
{
    stdio_init_all();
    
    draw.testPattern();
    display.update();

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
