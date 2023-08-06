#include "Print.hpp"

/**
 * @brief Construct a new Print object
 * @param display Display to print on
*/
Print::Print(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->width = params.width;
    this->height = params.height;
    this->totalPixels = params.width * params.height;
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
    this->cursor = (unsigned long)(point.x + point.y * this->width);
}

/**
 * @brief Move the cursor position
 * @param x X position to modify the cursor by
 * @param y Y position to modify the cursor by
*/
void Print::moveCursor(int x, int y)
{
    this->cursor += (unsigned long)(x + y * this->width);
}

/**
 * @brief Get the cursor position
 * @returns Cursor position
*/
Point Print::getCursor(void)
{
    return { (unsigned int)(this->cursor % this->width), (unsigned int)(this->cursor / this->width) };
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
 * @brief Print to the display
 * @note This behaves like printf
*/
void Print::print(const char* format, ...)
{
    // Generate the string
    va_list args;
    va_start(args, format);
    int size = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
    va_end(args);

    // loop through each character in the string
    for (int i = 0; i < size; i++)
    {
        // draw the character
        this->drawAscii(this->characterBuffer[i]);
    }
}

/**
 * @brief Get the width of a string in pixels
 * @note This behaves like printf
 * @returns Width of the string in pixels
*/
unsigned int Print::getStringWidth(const char* format, ...)
{
    // Generate the string
    va_list args;
    va_start(args, format);
    int size = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
    va_end(args);

    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int i = 0; i < size; i++)
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
 * @note This behaves like printf
 * @returns Height of the string in pixels
*/
unsigned int Print::getStringHeight(const char* format, ...)
{
    // Generate the string
    va_list args;
    va_start(args, format);
    int size = vsnprintf(this->characterBuffer, CHARACTER_BUFFER_SIZE - 1, format, args);
    va_end(args);

    // store the number of pixels
    size_t pixels = 0;

    // loop through each character in the string
    for (int i = 0; i < size; i++)
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
    if (!(character > 0x20 && character < 0x7E) && character != 0x0A && character != 0x0D && character != 0x09)
		return;
    
    // get the bitmap data
    const unsigned int* bitmap = this->font->bitmap;
    // get the character
    FontCharacter charData = this->font->characters[character - 0x20];

    // if the bitmap is a null pointer or overflows the frame buffer, return 0
    if (!((bitmap != nullptr) && ((charData.width * charData.height) < this->totalPixels)))
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
        this->cursor += (this->width - (this->cursor % this->width)) + this->width * this->font->newLineDistance;
        return;
    }
    else if(character == 0x0D) // carriage return
    {
        // move the cursor to the beginning of the line
        this->cursor -= (this->cursor % this->width);
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
    if (((bufferPosition % this->width) + charData.width) > this->width)
    {
        // move the cursor to the next line
        bufferPosition += (this->width - (this->cursor % this->width)) + this->width * this->font->newLineDistance;
    }
    // make sure the character is not placed off screen in the y direction, if so, return to the top
    if (((bufferPosition / this->width) + charData.height) > this->height)
    {
        // move the cursor to the top of the screen
        bufferPosition = 0;
    }

    // move the cursor by the y offset
    bufferPosition += charData.yOffset * this->width;
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
                bufferPosition += this->width;
            }
        }
    }

    // set the cursor to the end of the character
    this->cursor += rowSize;
}