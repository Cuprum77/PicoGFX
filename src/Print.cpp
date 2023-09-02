#include "Print.hpp"

/**
 * @brief Construct a new Print object
 * @param display Display to print on
*/
Print::Print(unsigned short* frameBuffer, display_config_t* config)
{
    this->frameBuffer = frameBuffer;
    this->config = config;
    this->color = Colors::White;
    this->font = nullptr;
    this->cursor = 0;
    this->characterBuffer[0] = '\0';
}

/**
 * @brief Set the color to use
 * @param color Color to use
*/
void Print::setColor(Color color)
{
	this->color = color.to16bit();
}

/**
 * @brief Get the color in use
*/
Color Print::getColor(void)
{
	return Color(this->color);
}

/**
 * @brief Set the cursor position
 * @param point Point to set the cursor to
*/
void Print::setCursor(Point point)
{
    this->cursor = (unsigned long)(point.x + point.y * this->config->width);
}

/**
 * @brief Move the cursor position
 * @param x X position to modify the cursor by
 * @param y Y position to modify the cursor by
*/
void Print::moveCursor(int x, int y)
{
    this->cursor += (unsigned long)(x + y * this->config->width);
}

/**
 * @brief Get the cursor position
 * @returns Cursor position
*/
Point Print::getCursor(void)
{
    return { (unsigned int)(this->cursor % this->config->width), (unsigned int)(this->cursor / this->config->width) };
}

/**
 * @brief Set the font to use
 * @param font Font to use
*/
void Print::setFont(FontStruct* font)
{
    this->font = font;
}

/**
 * @brief Put your string into the character buffer
 * @note This is the same as the printf function, but it puts the string into the buffer instead of printing it
 */
void Print::setString(const char* format, ...)
{
    // Generate the string and store the number of characters in the buffer
    va_list args;
    va_start(args, format);
    this->charactersInBuffer = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
    va_end(args);
}

/**
 * @brief Get the point to center the string at
 * @param alignment Alignment to use, defaults to Alignment_t::TotalCenter
 * @note This sets the cursor to center the string in the buffer
 */
void Print::center(Alignment_t alignment)
{
    // Get the center of the screen
    int centerX = this->config->width >> 1;
	int centerY = this->config->height >> 1;

    // Initialize the midpoint variables
    int stringWidthMidpoint = 0;
    int stringHeightMidpoint = 0;

    // Initialize the cursor variables
    unsigned long cursorX = 0;
    unsigned long cursorY = 0;

    // Set the cursor position based on the alignment
    switch (alignment)
    {
    case Alignment_t::HorizontalCenter:
        // Get the midpoint of the string
        stringWidthMidpoint = this->getStringWidth() >> 1;
        // Set the x position to the center, while keeping the y position the same
        cursorY = this->getCursor().y * this->config->width;
        this->cursor = (unsigned long)(centerX - stringWidthMidpoint + cursorY);
        break;
    case Alignment_t::VerticalCenter:
        // Get the midpoint of the string
        stringHeightMidpoint = this->getStringHeight() >> 1;
        // Move only the y position, keeping the x position the same
        cursorX = this->getCursor().x;
        this->cursor = (unsigned long)(cursorX + (centerY - stringHeightMidpoint) * this->config->width);
        break;
    case Alignment_t::TotalCenter:
    default:
        // Get the midpoint of the string
        stringWidthMidpoint = this->getStringWidth() >> 1;
        stringHeightMidpoint = this->getStringHeight() >> 1;
        // Set the x and y position to the center
        this->setCursor({ centerX - stringWidthMidpoint, centerY - stringHeightMidpoint });
        break;
    }
}

/**
 * @brief Print to the display
 * @note This behaves like printf
*/
void Print::print()
{
    // loop through each character in the string
    for (int i = 0; i < this->charactersInBuffer; i++)
    {
        // draw the character
        this->drawAscii(this->characterBuffer[i]);
    }
}

/**
 * @brief Get the width of a string in pixels
 * @returns Width of the string in the buffer, in pixels
*/
unsigned int Print::getStringWidth()
{
    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int i = 0; i < this->charactersInBuffer; i++)
    {
        // get the character
        char c = this->characterBuffer[i];

        // if the character is not printable, skip it
        if (!(this->characterBuffer[i] > 0x20 && this->characterBuffer[i] < 0x7E))
			continue;

        // get the character index by subtracting the offset
        c -= 0x20;
        // get the character data
        FontCharacter character = this->font->characters[c];
        // add the width of the character to the total
        pixels += character.width;
    }

    // return the number of pixels
    return pixels;
}

/**
 * @brief Get the height of a string in pixels
 * @returns Height of the string in the buffer, in pixels
*/
unsigned int Print::getStringHeight()
{
    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int i = 0; i < this->charactersInBuffer; i++)
    {
        // get the character
        char c = this->characterBuffer[i];

        // if the character is not printable, skip it
        if (!(this->characterBuffer[i] > 0x20 && this->characterBuffer[i] < 0x7E))
            continue;

        // get the character index by subtracting the offset
        c -= 0x20;
        // get the character data
        FontCharacter character = this->font->characters[c];
        // check if the height of the character is greater than the current height
        if(character.height > pixels)
			// set the height to the height of the character
            pixels = character.height;
    }

    // return the number of pixels
    return pixels;
}

/**
 * @brief Print to the display
 * @note This behaves like printf, and prints at the current cursor position
*/
void Print::print(const char* format, ...)
{
	// Generate the string
	va_list args;
	va_start(args, format);
	this->charactersInBuffer = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
	va_end(args);

	// loop through each character in the string
    for (int i = 0; i < this->charactersInBuffer; i++)
    {
		// draw the character
		this->drawAscii(this->characterBuffer[i]);
	}
}


/**
 * @private
 * @brief Draw an ascii character on the display
 * @param character Character to draw
 * @param Point Point to draw at
 * @param size Text size
 * @param color Color to draw the character
*/
void Print::drawAscii(const char character)
{
    // check if the font is a null pointer
    if(this->font == nullptr)
        return;

    // check if the character is valid
    if (!(character >= 0x20 && character <= 0x7E) && character != 0x0A && character != 0x0D && character != 0x09)
		return;
    
    // get the bitmap data
    const unsigned int* bitmap = this->font->bitmap;
    // get the character
    FontCharacter charData = this->font->characters[character - 0x20];

    // if the bitmap is a null pointer or overflows the frame buffer, return 0
    if (!((bitmap != nullptr) && ((charData.width * charData.height) < (this->config->width * this->config->height))))
        return;

    // handle edge cases
    if(character == 0x20) // space
    {
        // move the cursor by the width of the character
        this->cursor += charData.width;
        return;
    }
    else if(character == 0x0A) // new line
    {
        // move the cursor to the next line
        this->cursor += (this->config->width - (this->cursor % this->config->width)) + this->config->width * this->font->newLineDistance;
        return;
    }
    else if(character == 0x0D) // carriage return
    {
        // move the cursor to the beginning of the line
        this->cursor -= (this->cursor % this->config->width);
        return;
    }
    else if (character == 0x09) // tab
    {
        // move the cursor by the width of the character
        this->cursor += (charData.width * TAB_SIZE);
        return;
    }

    // get our current framebuffer pointer location
    unsigned int bufferPosition = this->cursor;
    // calculate the row size
    unsigned int rowSize = charData.width;

    // make sure the character is not placed off screen in the x direction, if so, move the character to the next line
    if (((bufferPosition % this->config->width) + charData.width) > this->config->width)
    {
        // move the cursor to the next line
        bufferPosition += (this->config->width - (this->cursor % this->config->width)) + this->config->width * this->font->newLineDistance;
    }
    // make sure the character is not placed off screen in the y direction, if so, return to the top
    if (((bufferPosition / this->config->width) + charData.height) > this->config->height)
    {
        // move the cursor to the top of the screen
        bufferPosition = 0;
    }

    // move the cursor by the y offset
    bufferPosition += charData.yOffset * this->config->width;
    // keep track of the current row position
    unsigned int rowPosition = 0;

    // loop constraints
    unsigned int loopEnd = charData.length - charData.pointer;

    // loop through the bitmap data
    for (int j = 0; j < loopEnd; j++)
    {
        // get the distance to move the cursor
        unsigned int data = bitmap[j + charData.pointer];

        // move the pointer by the number of pixels as defined by the distance
        for (int i = 0; i < data; i++)
        {
            // every other distance should be drawn, the first distance is always the number of pixels to skip
            if (j & 0x1) this->frameBuffer[rowPosition + bufferPosition] = this->color;

            // increment the row position
            rowPosition++;

            // if the row position is equal to the row size, we have completed a row
            if (rowPosition >= rowSize)
            {
                rowPosition = 0;
                bufferPosition += this->config->width;
            }
        }
    }

    // set the cursor to the end of the character
    this->cursor += rowSize;
}