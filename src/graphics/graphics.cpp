#include "graphics.h"

/**
 * @brief Construct a new graphics object
 * @param frameBuffer Pointer to the frame buffer
 * @param display_ptr Pointer to the display object
*/
graphics::graphics(uint16_t *frameBuffer, display *display_ptr)
{
    this->frameBuffer = frameBuffer;
    this->display_ptr = display_ptr;
}

/**
 * @brief Fill the display with a color
 * @param color color to fill with
*/
void graphics::fill(color color)
{
    // convert color to 16 bit
    uint16_t color16 = color.to16bit();
	uint32_t totalPixels = this->display_ptr->getWidth() * this->display_ptr->getHeight();
    // fill the frame buffer
    for (int32_t i = 0; i < totalPixels; i++)
        this->frameBuffer[i] = color16;
}

/**
 * @brief Fill the display with a color
 * @param color color to fill with
*/
void graphics::fill(uint16_t color)
{
	uint16_t color16 = color;

#if defined(LCD_INVERT_COLORS)
		color16 = ((color16 & 0xaaaa) >> 1) | ((color16 & 0x5555) << 1);
		color16 = ((color16 & 0xcccc) >> 2) | ((color16 & 0x3333) << 2);
		color16 = ((color16 & 0xf0f0) >> 4) | ((color16 & 0x0f0f) << 4);
		color16 = (color16 >> 8) | (color16 << 8);
#endif

	uint32_t totalPixels = this->display_ptr->getWidth() * this->display_ptr->getHeight();
    // fill the frame buffer
    for (int32_t i = 0; i < totalPixels; i++)
        this->frameBuffer[i] = color16;
}

/**
 * @brief Fill the display with a test pattern
*/
void graphics::testPattern(void)
{
	uint32_t width = this->display_ptr->getWidth();
	uint32_t height = this->display_ptr->getHeight();

	color top_row[] = {
		colors::argent, colors::acidGreen, colors::turquoiseSurf,
		colors::islamicGreen, colors::deepMagenta, colors::ueRed,
		colors::mediumBlue
	};

	color middle_row[] = {
		colors::mediumBlue, colors::chineseBlack, colors::deepMagenta,
		colors::chineseBlack, colors::turquoiseSurf, colors::chineseBlack,
		colors::argent
	};

	color bottom_row[] = {
		colors::oxfordBlue, colors::white, colors::deepViolet,
		colors::chineseBlack, colors::chineseBlack
	};

	color bottom_black_band[] = {
		colors::vampireBlack, colors::chineseBlack, colors::eerieBlack
	};

	uint32_t top_height = 0;
	uint32_t middle_height = (height * 2) / 3;
	uint32_t bottom_height = (height * 3) / 4;

	uint32_t top_segment = width / 7;
	uint32_t middle_segment = width / 7;

	uint32_t bottom_large_width = (width * 5) / 7;
	uint32_t bottom_small_width = width - bottom_large_width;

	uint32_t bottom_small_segment = bottom_small_width / 3;

	for (size_t i = 0; i < 7; i++)
	{
		uint32_t x_start = top_segment * i;
		uint32_t x_end = (i == 6) ? width : top_segment * (i + 1);

		point start(x_start, top_height);
		point end(x_end, middle_height);

		this->drawFilledRectangle(start, end, top_row[i]);
	}

	for (size_t i = 0; i < 7; i++)
	{
		uint32_t x_start = middle_segment * i;
		uint32_t x_end = (i == 6) ? width : middle_segment * (i + 1);

		point start(x_start, middle_height);
		point end(x_end, bottom_height);

		this->drawFilledRectangle(start, end, middle_row[i]);
	}

	uint32_t bottom_large_segment = bottom_large_width / 4;

	for (size_t i = 0; i < 4; i++)
	{
		uint32_t x_start = bottom_large_segment * i;
		uint32_t x_end = (i == 3) ? bottom_large_width : bottom_large_segment * (i + 1);

		point start(x_start, bottom_height);
		point end(x_end, height);

		this->drawFilledRectangle(start, end, bottom_row[i]);
	}

	for (size_t i = 0; i < 3; i++)
	{
		uint32_t x_start = bottom_large_width + (bottom_small_segment * i);
		uint32_t x_end = (i == 2) ? width : bottom_large_width + (bottom_small_segment * (i + 1));

		point start(x_start, bottom_height);
		point end(x_end, height);

		this->drawFilledRectangle(start, end, bottom_black_band[i]);
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
    int32_t index = x + y * this->display_ptr->getWidth();

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