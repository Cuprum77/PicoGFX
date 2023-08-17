#include "PicoGFX.hpp"

/**
 * @brief Get the Display object
 * @return Display&
*/
Display& PicoGFX::getDisplay()
{
    return *display;
}

/**
 * @brief Get the Print object
 * @return Print&
*/
Print& PicoGFX::getPrint()
{
    return *print;
}

/**
 * @brief Get the Graphics object
 * @return Graphics&
*/
Graphics& PicoGFX::getGraphics()
{
    return *graphics;
}

/**
 * @brief Get the Gradients object
 * @return Gradients&
*/
Gradients& PicoGFX::getGradients()
{
    return *gradients;
}

/**
 * @brief Get the Encoder object
 * @return Encoder&
*/
Encoder& PicoGFX::getEncoder()
{
    return *encoder;
}


/**
 * @brief Get the Display pointer
 * @return Display*
*/
Display* PicoGFX::getDisplayPtr()
{
    return display;
}

/**
 * @brief Get the Print pointer
 * @return Print*
*/
Print* PicoGFX::getPrintPtr()
{
    return print;
}

/**
 * @brief Get the Graphics pointer
 * @return Graphics*
*/
Graphics* PicoGFX::getGraphicsPtr()
{
    return graphics;
}

/**
 * @brief Get the Gradients pointer
 * @return Gradients*
*/

Gradients* PicoGFX::getGradientsPtr()
{
    return gradients;
}

/**
 * @brief Get the Encoder pointer
 * @return Encoder*
*/
Encoder* PicoGFX::getEncoderPtr()
{
    return encoder;
}