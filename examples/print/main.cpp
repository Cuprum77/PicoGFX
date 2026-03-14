#include <stdio.h>
#include "pico/stdlib.h"
#include "picogfx.h"

int main()
{
    stdio_init_all();
    
    draw.testPattern();
    display.update();

    sleep_ms(1000);

    while (true)
    {
        draw.fill(colors::black);

        print.setString("Hello, World!\n");
        print.setFont(&RobotoMono16);
        print.setColor(colors::cyan);
        print.setCursor({0, 0});
        print.center(Alignment_t::HorizontalCenter);
        print.print();

        print.setColor(colors::magenta);
        print.print("Magenta text!\n");
        print.setColor(colors::red);
        print.print("Red text!\n");
        print.print("We \ttab!\n");

        print.setString("%d FPS\n", display.getFrameCounter());
        print.setFont(&RobotoMono48);
        print.setColor(colors::lightGreen);
        print.center(Alignment_t::TotalCenter);
        print.print();        
        
        print.setString("\nThis is a number: %d\n", 123);
        print.setFont(&RobotoMono16);
        print.setColor(colors::derg);
        print.print();

        print.setString("This is a float: %.2f\n", 3.14159);
        print.setColor(colors::raspberryRed);
        print.print();

        print.setString("Woah, a pointer: %p\n", &print);
        print.setColor(colors::greenYellow);
        print.print();

        // Print rainbow text
        const char *rainbowText = "Rainbow Text!";
        const color rainbowColors[] = { colors::red, colors::orange, colors::yellow, colors::green, colors::cyan, colors::blue, colors::magenta };
        print.setFont(&RobotoMono24);
        for (size_t i = 0; rainbowText[i] != '\0'; i++)
        {
            print.setString("%c", rainbowText[i]);
            print.setColor(rainbowColors[i % (sizeof(rainbowColors) / sizeof(rainbowColors[0]))]);
            print.print();
        }

        display.update();
        display.frameCounter();
    }

    return 0;
}
