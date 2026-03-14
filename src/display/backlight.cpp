#include "display.h"
#if defined(LCD_BACKLIGHT_ENABLED)

#if defined(LCD_BACKLIGHT_ENABLED)
/**
 * @brief Initialize the backlight
 * @note Defaults to 50% brightness if dimmable, otherwise defaults to on
 */
void display_obj::initBacklight()
{
#if defined(LCD_BACKLIGHT_DIMMABLE)
    // enable dimming
    gpio_set_function(LCD_PIN_BL, GPIO_FUNC_PWM);
    // get the PWM slice number
    uint32_t sliceNum = pwm_gpio_to_slice_num(LCD_PIN_BL);
    this->sliceNum = sliceNum;
    // get the PWM channel
    uint32_t chan = pwm_gpio_to_channel(LCD_PIN_BL);
    this->pwmChannel = chan;
    // turn on the PWM slice
    pwm_set_enabled(sliceNum, true);
    // set the PWM wrap value
    pwm_set_wrap(sliceNum, 255);
    // set the PWM value
    pwm_set_chan_level(sliceNum, chan, 127);
#else
    gpio_init(LCD_PIN_BL);
    gpio_set_dir(LCD_PIN_BL, GPIO_OUT);
    gpio_put(LCD_PIN_BL, 1);
#endif
}

#if defined(LCD_BACKLIGHT_DIMMABLE)
/**
 * @brief Set the backlight brightness
 * @param brightness Brightness to set (0-100%)
 * @return None
 */
void display_obj::setBrightness(uint8_t brightness)
{
    uint32_t brightness32 = brightness;
#if defined(LCD_BACKLIGHT_INV)
    brightness32 = 100 - brightness32;
#endif
    // handle overflow and underflow
    if (brightness32 > 100)
        brightness32 = 100;

    // create a brightness curve so that the brightness changes more smoothly to the human eye
    brightness32 = (brightness32 * brightness32) / 100;

    // convert brightness from 0-100 to 0-255
    brightness32 = (brightness32 * 255) / 100;

    pwm_set_chan_level(this->sliceNum, this->pwmChannel, brightness32);
    this->brightness = brightness;
}

/**
 * @brief Set the backlight brightness
 * @param brightness Brightness to set (0-255)
 * @return None
 */
void display_obj::setBrightnessRaw(uint8_t brightness)
{
#if defined(LCD_BACKLIGHT_INV)
    brightness = 255 - brightness;
#endif
    pwm_set_chan_level(this->sliceNum, this->pwmChannel, brightness);
    this->brightness = brightness;
}

/**
 * @brief Get the backlight brightness
 * @return Brightness (0-255)
 */
uint8_t display_obj::getBrightness(void)
{
    return this->brightness;
}

#else
/**
 * @brief Set the backlight on or off
 * @param on true to turn on, false to turn off
 * @return None
 */
void display_obj::setBrightness(bool on)
{
#if defined(LCD_BACKLIGHT_INV)
    on = !on;
#endif
    gpio_put(LCD_PIN_BL, on);
    this->backlight = on;
}

/**
 * @brief Get the backlight state
 * @return true if on, false if off
 */
bool display_obj::getBrightness(void)
{
    return this->backlight;
}

#endif
#endif

#endif