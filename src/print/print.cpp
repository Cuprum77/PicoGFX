#include "print.h"

/**
 * @brief Construct a new print object
 * @param display Display to print on
*/
printer::printer(uint16_t *frameBuffer, display_obj *display_ptr)
{
    this->frameBuffer = frameBuffer;
    this->color_val = colors::black;
    this->font = nullptr;
    this->display_ptr = display_ptr;
    this->cursor = 0;
    this->characterBuffer[0] = '\0';
}

/**
 * @brief Set the color to use
 * @param color color to use
*/
void printer::setColor(color val)
{
	this->color_val = val.toWord();
}

/**
 * @brief Get the color in use
*/
color printer::getColor(void)
{
	return color(this->color_val);
}

/**
 * @brief Set the cursor position
 * @param point point to set the cursor to
*/
void printer::setCursor(point point)
{
    this->cursor = (uint32_t)(point.x + point.y * this->display_ptr->getWidth());
}

/**
 * @brief Move the cursor position
 * @param x X position to modify the cursor by
 * @param y Y position to modify the cursor by
*/
void printer::moveCursor(int32_t x, int32_t y)
{
    this->cursor += (uint32_t)(x + y * this->display_ptr->getWidth());
}

/**
 * @brief Get the cursor position
 * @returns Cursor position
*/
point printer::getCursor(void)
{
    return { (uint32_t)(this->cursor % this->display_ptr->getWidth()), (uint32_t)(this->cursor / this->display_ptr->getWidth()) };
}

/**
 * @brief Set the font to use
 * @param font Font to use
*/
void printer::setFont(FontStruct *font)
{
    this->font = font;
}

/**
 * @brief Put your string into the character buffer
 * @note This is the same as the printf function, but it puts the string into the buffer instead of printing it
 */
void printer::setString(const char *format, ...)
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
void printer::center(Alignment_t alignment)
{
    // Get the center of the screen
    int32_t centerX = this->display_ptr->getWidth() >> 1;
	int32_t centerY = this->display_ptr->getHeight() >> 1;

    // Initialize the midpoint variables
    int32_t stringWidthMidpoint = 0;
    int32_t stringHeightMidpoint = 0;

    // Initialize the cursor variables
    uint32_t cursorX = 0;
    uint32_t cursorY = 0;

    // Set the cursor position based on the alignment
    switch (alignment)
    {
    case Alignment_t::HorizontalCenter:
        // Get the midpoint of the string
        stringWidthMidpoint = this->getStringWidth() >> 1;
        // Set the x position to the center, while keeping the y position the same
        cursorY = this->getCursor().y * this->display_ptr->getWidth();
        this->cursor = (uint32_t)(centerX - stringWidthMidpoint + cursorY);
        break;
    case Alignment_t::VerticalCenter:
        // Get the midpoint of the string
        stringHeightMidpoint = this->getStringHeight() >> 1;
        // Move only the y position, keeping the x position the same
        cursorX = this->getCursor().x;
        this->cursor = (uint32_t)(cursorX + (centerY - stringHeightMidpoint) * this->display_ptr->getWidth());
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
 * @brief print to the display
 * @note This behaves like printf
*/
void printer::print()
{
    // loop through each character in the string
    for (int32_t i = 0; i < this->charactersInBuffer; i++)
    {
        // draw the character
        this->drawAscii(this->characterBuffer[i]);
    }
}

/**
 * @brief Get the width of a string in pixels
 * @returns Width of the string in the buffer, in pixels
*/
uint32_t printer::getStringWidth()
{
    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int32_t i = 0; i < this->charactersInBuffer; i++)
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
uint32_t printer::getStringHeight()
{
    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int32_t i = 0; i < this->charactersInBuffer; i++)
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
 * @brief print to the display
 * @note This behaves like printf, and prints at the current cursor position
*/
void printer::print(const char *format, ...)
{
	// Generate the string
	va_list args;
	va_start(args, format);
	this->charactersInBuffer = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
	va_end(args);

	// loop through each character in the string
    for (int32_t i = 0; i < this->charactersInBuffer; i++)
    {
		// draw the character
		this->drawAscii(this->characterBuffer[i]);
	}
}


/**
 * @private
 * @brief Draw an ascii character on the display
 * @param character Character to draw
 * @param point point to draw at
 * @param size Text size
 * @param color color to draw the character
*/
void printer::drawAscii(const char character)
{
    // check if the font is a null pointer
    if(this->font == nullptr)
        return;

    // check if the character is valid
    if (!(character >= 0x20 && character <= 0x7E) && character != 0x0A && character != 0x0D && character != 0x09)
		return;
    
    // get the bitmap data
    const uint32_t *bitmap = this->font->bitmap;
    // get the character
    FontCharacter charData = this->font->characters[character - 0x20];

    // if the bitmap is a null pointer or overflows the frame buffer, return 0
    if (!((bitmap != nullptr) && ((charData.width * charData.height) < (this->display_ptr->getWidth() * this->display_ptr->getHeight()))))
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
        this->cursor += (this->display_ptr->getWidth() - (this->cursor % this->display_ptr->getWidth())) + this->display_ptr->getWidth() * this->font->newLineDistance;
        return;
    }
    else if(character == 0x0D) // carriage return
    {
        // move the cursor to the beginning of the line
        this->cursor -= (this->cursor % this->display_ptr->getWidth());
        return;
    }
    else if (character == 0x09) // tab
    {
        // move the cursor by the width of the character
        this->cursor += (charData.width * TAB_SIZE);
        return;
    }

    // get our current framebuffer pointer location
    uint32_t bufferPosition = this->cursor;
    // calculate the row size
    uint32_t rowSize = charData.width;

    // make sure the character is not placed off screen in the x direction, if so, move the character to the next line
    if (((bufferPosition % this->display_ptr->getWidth()) + charData.width) > this->display_ptr->getWidth())
    {
        // move the cursor to the next line
        bufferPosition += (this->display_ptr->getWidth() - (this->cursor % this->display_ptr->getWidth())) + this->display_ptr->getWidth() * this->font->newLineDistance;
    }
    // make sure the character is not placed off screen in the y direction, if so, return to the top
    if (((bufferPosition / this->display_ptr->getWidth()) + charData.height) > this->display_ptr->getHeight())
    {
        // move the cursor to the top of the screen
        bufferPosition = 0;
    }

    // move the cursor by the y offset
    bufferPosition += charData.yOffset * this->display_ptr->getWidth();
    // keep track of the current row position
    uint32_t rowPosition = 0;

    // loop constraints
    uint32_t loopEnd = charData.length - charData.pointer;

    // loop through the bitmap data
    for (int32_t j = 0; j < loopEnd; j++)
    {
        // get the distance to move the cursor
        uint32_t data = bitmap[j + charData.pointer];

        // move the pointer by the number of pixels as defined by the distance
        for (int32_t i = 0; i < data; i++)
        {
            // every other distance should be drawn, the first distance is always the number of pixels to skip
            if (j & 0x1) this->frameBuffer[rowPosition + bufferPosition] = this->color_val;

            // increment the row position
            rowPosition++;

            // if the row position is equal to the row size, we have completed a row
            if (rowPosition >= rowSize)
            {
                rowPosition = 0;
                bufferPosition += this->display_ptr->getWidth();
            }
        }
    }

    // set the cursor to the end of the character
    this->cursor += rowSize;
}