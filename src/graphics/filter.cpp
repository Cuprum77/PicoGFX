#include "graphics.hpp"

#define ERR_RIGHT  7
#define ERR_DOWN   5
#define ERR_DOWN_L 3
#define ERR_DOWN_R 1

/**
 * @brief Add a Bayer filter to the display
 */
void graphics::addBayerFilter(void)
{
    // loop through each and every pixel
    uint16_t* ptr = this->frameBuffer;
    for (size_t y = 0; y < this->config->height; y++)
    {
        for (size_t x = 0; x < this->config->width; x++)
        {
            uint8_t threshold = this->bayerMatrix[(x & 3) + ((y & 3) << 2)];
            color c = *ptr;

            // threshold the color
            c.r += (threshold >> 4);
            c.g += (threshold >> 3);
            c.b += (threshold >> 4);

            // clamp the color
            c.r &= 0x1f;
            c.g &= 0x3f;
            c.b &= 0x1f;

            // set the pixel
            *ptr++ = c.to16bit(this->config->inverseColors);
        }
    }
}

/**
 * @brief Add a Floyd-Steinberg dithering filter to the display
 */
void graphics::addFloydSteinbergDithering(void)
{
    int16_t error_buffer[this->config->width * 3] = {0};
    
    for (int y = 0; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int i = y * width + x;

            // Extract RGB565 components
            uint16_t pixel = this->frameBuffer[i];
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5)  & 0x3F;
            uint8_t b = pixel & 0x1F;

            // Apply stored error
            r = (r + (error_buffer[i * 3]   >> 4)) & 0x1F;
            g = (g + (error_buffer[i * 3+1] >> 4)) & 0x3F;
            b = (b + (error_buffer[i * 3+2] >> 4)) & 0x1F;

            // Quantize to RGB565
            uint16_t new_pixel = (r << 11) | (g << 5) | b;
            this->frameBuffer[i] = new_pixel;

            // Compute error
            int16_t err_r = (r << 3) - ((new_pixel >> 11) & 0x1F) * 8;
            int16_t err_g = (g << 2) - ((new_pixel >> 5)  & 0x3F) * 4;
            int16_t err_b = (b << 3) - (new_pixel & 0x1F) * 8;

            // Diffuse error (scaled for integer math)
            error_buffer[(i + 1) * 3]   += err_r * ERR_RIGHT;
            error_buffer[(i + 1) * 3+1] += err_g * ERR_RIGHT;
            error_buffer[(i + 1) * 3+2] += err_b * ERR_RIGHT;

            error_buffer[(i + width - 1) * 3]   += err_r * ERR_DOWN_L;
            error_buffer[(i + width - 1) * 3+1] += err_g * ERR_DOWN_L;
            error_buffer[(i + width - 1) * 3+2] += err_b * ERR_DOWN_L;

            error_buffer[(i + width) * 3]   += err_r * ERR_DOWN;
            error_buffer[(i + width) * 3+1] += err_g * ERR_DOWN;
            error_buffer[(i + width) * 3+2] += err_b * ERR_DOWN;

            error_buffer[(i + width + 1) * 3]   += err_r * ERR_DOWN_R;
            error_buffer[(i + width + 1) * 3+1] += err_g * ERR_DOWN_R;
            error_buffer[(i + width + 1) * 3+2] += err_b * ERR_DOWN_R;
        }
    }
}

/**
 * @brief Apply a simple blur to the display to combat pixelation
*/
void graphics::addAntiAliasingFilter(void)
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

/**
 * @brief Apply a blur to the entire display
 */
void graphics::addBlur(void)
{
    rect area = rect(0, 0, this->config->width, this->config->height);
    this->addBlur(area);
}

/**
 * @brief Apply a blur to a specific area of the display
 * @param area Area to apply the blur to
 */
void graphics::addBlur(rect area)
{
    for (int32_t y = 1; y < this->config->height - 1; y++) 
    {
        for (int32_t x = 1; x < this->config->width - 1; x++) 
        {
            int32_t i = x + y * this->config->width;

            // Sum up 3x3 neighborhood
            uint32_t r_sum = 0, g_sum = 0, b_sum = 0;
            for (int32_t dy = -1; dy <= 1; dy++) 
            {
                for (int32_t dx = -1; dx <= 1; dx++) 
                {
                    uint16_t p = this->frameBuffer[i + dx + dy * width];
                    r_sum += (p >> 11) & 0x1F;
                    g_sum += (p >> 5) & 0x3F;
                    b_sum += p & 0x1F;
                }
            }

            // Approximate division by 9 using bit shifts (36/512 ≈ 9/256)
            uint8_t r = (r_sum * 36) >> 9;
            uint8_t g = (g_sum * 36) >> 9;
            uint8_t b = (b_sum * 36) >> 9;

            // Write back the blurred pixel
            this->frameBuffer[i] = (r << 11) | (g << 5) | b;
        }
    }
}