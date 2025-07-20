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
 * @brief Fill the display with a test pattern
*/
void graphics::testPattern(void)
{
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
	uint32_t middle_height = (this->height / 3) * 2;
	uint32_t bottom_height = (this->height / 4) * 3;

	uint32_t top_width = this->width / 7;
	uint32_t middle_width = this->width / 7;
	uint32_t bottom_width1 = (middle_width * 5) / 4;
	uint32_t bottom_width2 = middle_width / 3;

	for (size_t i = 0; i < 7; i++)
	{
		uint32_t x_start = top_width * i;
		uint32_t x_end = (i == 6) ? this->width : top_width * (i + 1);
		point start = point(x_start, top_height);
		point end = point(x_end, imin(middle_height, this->height));
		this->drawFilledRectangle(start, end, top_row[i]);
	}

	for (size_t i = 0; i < 7; i++)
	{
		uint32_t x_start = middle_width * i;
		uint32_t x_end = (i == 6) ? this->width : middle_width * (i + 1);
		point start = point(x_start, middle_height);
		point end = point(x_end, imin(bottom_height, this->height));
		this->drawFilledRectangle(start, end, middle_row[i]);
	}

	for (size_t i = 0; i < 4; i++)
	{
		uint32_t x_start = bottom_width1 * i;
		uint32_t x_end = (i == 3) ? this->width : bottom_width1 * (i + 1);
		point start = point(x_start, bottom_height);
		point end = point(x_end, this->height);
		this->drawFilledRectangle(start, end, bottom_row[i]);
	}

	for (size_t i = 0; i < 3; i++)
	{
		uint32_t x_start = (middle_width * 5) + (this->width - (middle_width * 6)) / 3 * i;
		uint32_t x_end = (i == 2) ? this->width : x_start + (this->width - (middle_width * 5)) / 3;
		point start = point(x_start, bottom_height);
		point end = point(x_end, this->height);
		this->drawFilledRectangle(start, end, bottom_black_band[i]);
	}

	point start = point((middle_width * 6), bottom_height);
	point end = point(this->width, this->height);
	this->drawFilledRectangle(start, end, bottom_row[4]);
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