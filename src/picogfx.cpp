#include "picogfx.h"

hardware_driver  _hw;
display_driver_t display(&_hw);

#if !defined(TOUCH_DISABLED)
touch_driver_t touch(&_hw);
#endif

#if defined(PICO_GFX_GRAPHICS)
graphics draw(display.getFrameBuffer(), &display);
#endif

#if defined(PICO_GFX_PRINT)
printer print(display.getFrameBuffer(), &display);
#endif

#if defined(PICO_GFX_GAUGE)
// does nothing for now
#endif

#if defined(PICO_GFX_GRADIENT)
gradient_obj gradient(display.getFrameBuffer(), &display);
#endif

extern "C" void __real_stdio_init_all();

extern "C" void __wrap_stdio_init_all()
{
    __real_stdio_init_all();
    _hw.init();
    display.init();
#if !defined(TOUCH_DISABLED)
    touch.init();
#endif
}