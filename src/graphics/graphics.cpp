#include "graphics.hpp"

/**
 * @brief Construct a new graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param params Display parameters
*/
graphics::graphics(uint16_t* frameBuffer, display_config_t* config)
{
    this->frameBuffer = frameBuffer;
    this->config = config;
    this->totalPixels = config->width * config->height;
    fillLookupTables();
}

/**
 * @brief Fill the display with a color
 * @param color color to fill with
*/
void graphics::fill(color color)
{
    // convert color to 16 bit
    uint16_t color16 = color.to16bit();
    // fill the frame buffer
    for (int32_t i = 0; i < this->totalPixels; i++)
        this->frameBuffer[i] = color16;
}

/**
 * @brief Apply a simple blur to the display to combat pixelation
*/
void graphics::antiAliasingFilter(void)
{
    // Helper function to get the color difference between two pixels
    #define COLOR_DIFF(pixel1, pixel2) ( \
        iabs(((pixel1 & 0xF800) >> 8) - ((pixel2 & 0xF800) >> 8)) + \
        iabs(((pixel1 & 0x07E0) >> 3) - ((pixel2 & 0x07E0) >> 3)) + \
        iabs(((pixel1 & 0x001F) << 3) - ((pixel2 & 0x001F) << 3)) \
    )

    // Loop through all the pixels in the framebuffer
    for (int32_t y = 1; y < (this->config->height - 1); y++)
    {
        for (int32_t x = 1; x < (this->config->width - 1); x++)
        {
            // Get the index of the current pixel
            int32_t pixelIndex = y * this->config->width + x;

            // Get the surrounding pixels
            uint16_t leftPixel = this->frameBuffer[pixelIndex - 1];
            uint16_t rightPixel = this->frameBuffer[pixelIndex + 1];
            uint16_t upPixel = this->frameBuffer[pixelIndex - this->config->width];
            uint16_t downPixel = this->frameBuffer[pixelIndex + this->config->width];            

            // Calculate the  difference between the left and right pixel and the up and down pixel
            int32_t horizontalDiff = COLOR_DIFF(leftPixel, rightPixel);
            int32_t verticalDiff = COLOR_DIFF(upPixel, downPixel);

            // If the difference is large enough, average the colors
            if (horizontalDiff > 16 && verticalDiff > 16)
            {
                // Get the sum of the colors
                int32_t sumR = ((leftPixel & 0xF800) + (rightPixel & 0xF800) + (upPixel & 0xF800) + (downPixel & 0xF800)) >> 8;
                int32_t sumG = ((leftPixel & 0x07E0) + (rightPixel & 0x07E0) + (upPixel & 0x07E0) + (downPixel & 0x07E0)) >> 3;
                int32_t sumB = ((leftPixel & 0x001F) + (rightPixel & 0x001F) + (upPixel & 0x001F) + (downPixel & 0x001F)) << 3;
                // Average the colors
                uint16_t avgR = (sumR >> 2) & 0xF8;
                uint16_t avgG = (sumG >> 2) & 0xFC;
                uint16_t avgB = (sumB >> 2) >> 3;
                // Write the averaged color to the framebuffer
                this->frameBuffer[pixelIndex] = (avgR << 8) | (avgG << 3) | avgB;
            }
        }
    }
}

//   rrrrrggggggbbbbb
#define RB_MASK      63519  // 0b1111100000011111        --> hex :F81F
#define G_MASK        2016  // 0b0000011111100000        --> hex :07E0
#define RB_MUL_MASK 2032608 // 0b111110000001111100000   --> hex :1F03E0
#define G_MUL_MASK   64512  // 0b000001111110000000000   --> hex :FC00

/**
 * @private
 * @brief Helper function to blend the pixel color with the background for anti-aliasing
 * @param x X coordinate of the pixel to blend
 * @param y Y coordinate of the pixel to blend
 * @param color color of the pixel to blend
 * @param alpha Alpha value of the pixel to blend, 0 is transparent, 255 is opaque
 * @note Sets the pixel at the given index to the blended color
 */
void graphics::setPixelBlend(uint32_t x, uint32_t y, uint16_t color, uint8_t alpha)
{
    // Taken from this stackoverflow answer https://stackoverflow.com/questions/72456587/implement-a-function-that-blends-two-colors-encoded-with-rgb565-using-alpha-blen

    // Get the buffer index
    int32_t index = x + y * this->config->width;

    // Reduce the alpha from [0,255] to [0,31] 
    alpha = alpha >> 0x3;
    // Create the inverse alpha value and call it beta
    uint8_t beta = 0x20 - alpha;

    // Get the background color
    uint16_t background = this->frameBuffer[index];

    // Apply the alpha blending formula
    uint16_t result = (uint16_t)((((alpha * (uint32_t)(color & RB_MASK) + beta * (uint32_t)(background & RB_MASK)) & RB_MUL_MASK)
        | ((alpha * (color & G_MASK) + beta * (background & G_MASK)) & G_MUL_MASK)) >> 0x5);

    // Apply the blended color to the framebuffer
    this->frameBuffer[index] = result;
}