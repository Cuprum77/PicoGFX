#pragma once

#ifdef __cplusplus

#include "lcd_config.h"
#include "driver.h"
#include "shapes.h"

#if defined(LCD_DRIVER_GC9A01)
    #include "gc9a01.h"
    using display_driver_t = gc9a01;
#elif defined(LCD_DRIVER_ST7789)
    #include "st7789.h"
    using display_driver_t = st7789;
#elif defined(LCD_DRIVER_CNA3306)
    #include "cna3306.h"
    using display_driver_t = cna3306;
#else
    #error "No display driver defined"
#endif

#if !defined(TOUCH_DISABLED)
#if defined(TOUCH_DRIVER_CST816) 
        #include "cst816.h"
        using touch_driver_t = cst816;
#elif defined(TOUCH_DRIVER_GT911) 
        #include "gt911.h"
        using touch_driver_t = gt911;
#else
    #error "No touch driver defined"
#endif
    extern touch_driver_t   touch;
#endif

#if defined(PICO_GFX_GRAPHICS)
    #include "graphics.h"
    extern graphics draw;
#endif

#if defined(PICO_GFX_PRINT)
    #include "print.h"
    #include "pico_gfx_fonts.h"
    extern printer print;
#endif

#if defined(PICO_GFX_GAUGE)
// does nothing for now
#endif

#if defined(PICO_GFX_GRADIENT)
    #include "gradient.h"
    extern gradient_obj gradient;
#endif

extern hardware_driver  _hw;
extern display_driver_t display;
#endif