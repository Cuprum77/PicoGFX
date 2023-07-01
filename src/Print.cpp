#include "Print.hpp"

/**
 * @brief Construct a new Print object
 * @param display Display to print on
*/
Print::Print(unsigned short* frameBuffer, Display_Params params)
{
    this->frameBuffer = frameBuffer;
    this->params = params;
    this->totalPixels = params.width * params.height;
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::print(long number, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer, size);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::print(unsigned long number, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->print(buffer, size);
}

/**
 * @brief Write a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Print::print(double number, unsigned char size, unsigned int precision)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE] = {0};    // largest number a double can represent is 1.79769e+308
    this->floatToString(number, buffer, precision);
    // write the string
    this->print(buffer, size);
}

/**
 * @brief Write a character on the display
 * @param character Character to print
 * @param color Character color
 * @param size Size of the character
*/
void Print::print(const char* text, unsigned char size)
{
    // get the length of the text
    unsigned int length = strlen(text);
    unsigned int x = this->cursor % this->params.width;
    unsigned int y = this->cursor / this->params.width;

    // loop through the text
    for(int i = 0; i < length; i++)
    {
        // if the text is a new line, move the text to the next line
        if (text[i] == '\n')
        {
            this->cursor = this->cursor - (this->cursor % this->params.width) + this->params.width * (FONT_HEIGHT * size);
            continue;
        }
        // if the text is a tab move the text to the next tab stop
        else if (text[i] == '\t')
        {
            this->cursor += FONT_WIDTH * size * TAB_SIZE;
            continue;
        }
        // check if the text is going to go off the screen by checking the future x Point with the width of the screen
        else if ((x + FONT_WIDTH * size) > this->params.width)
        {
            // move the text to the next line
            this->cursor = this->cursor - (this->cursor % this->params.width) + this->params.width * (FONT_HEIGHT * size);
        }
        // if we overflowed the screen, begin from the top again
        if ((y + FONT_HEIGHT * size) > this->params.height)
        {
            this->cursor = 0;
        }

        // draw the character
        this->drawAscii(text[i], size);
    }
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::println(long number, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->println(buffer, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param size Size of the number
*/
void Print::println(unsigned long number, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(number, buffer, base);
    // write the string
    this->println(buffer, size);
}

/**
 * @brief Print a number on the display
 * @param number Number to print
 * @param precision Number of decimal places to print
 * @param size Size of the number
*/
void Print::println(double number, unsigned char size, unsigned int precision)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE] = {0};    // largest number a double can represent is 1.79769e+308
    this->floatToString(number, buffer, precision);
    // write the string
    this->println(buffer, size);
}

/**
 * @brief Print a character on the display
 * @param character Character to print
 * @param size Size of the character
*/
void Print::println(const char* text, unsigned char size)
{
    this->print(text, size);
    this->print("\n", size);
}

/**
 * @brief Write a character on the display
 * @param value Boolean to print
*/
void Print::println(bool value, unsigned char size)
{
    this->println(value ? TRUE : FALSE, size);
}

/**
 * @brief Print a newline
*/
void Print::println(void)
{
    this->println("\n");
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(long num, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(num, buffer, base);
    unsigned int len = strlen(buffer);
    return (len * size * FONT_WIDTH);
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(unsigned long num, unsigned char size, number_base_t base)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    itoa(num, buffer, base);
    unsigned int len = strlen(buffer);
    return (len * size * FONT_WIDTH);
}

/**
 * @brief Get the length of a string
 * @param num String to get the length of
 * @param precision Number of decimal places to print
 * @param size Size of the string
 * @param base Base of the number
 * @return Length of the string
*/
unsigned int Print::getStringLength(double num, unsigned char precision, unsigned char size)
{
    // convert the number to a string
    char buffer[CHARACTER_BUFFER_SIZE];    // largest number a long can represent is 9 223 372 036 854 775 807
    floatToString(num, buffer, precision);
    unsigned int len = strlen(buffer);
    return (len * size * FONT_WIDTH);
}

/**
 * @brief Get the length of a string
 * @param text String to get the length of
 * @param size Size of the string
 * @return Length of the string
*/
unsigned int Print::getStringLength(const char* text, unsigned char size)
{
    return (strlen(text) * size * FONT_WIDTH);
}

/**
 * @brief Get the length of a string
 * @param value String to get the length of
 * @param size Size of the string
 * @return Length of the string
*/
unsigned int Print::getStringLength(bool value, unsigned char size)
{
    if(value)
        return (strlen(TRUE) * size * FONT_WIDTH);
    else
        return (strlen(FALSE) * size * FONT_WIDTH);
}

/**
 * @private
 * @brief Convert float to string
 * @param value Value to convert
 * @param buffer Buffer to write to
 * @param precision Precision of the value
*/
void Print::floatToString(double num, char* buffer, unsigned int precision)
{
    // if precision is 0, just return the integer part
    if(precision == 0)
    {
        itoa((long)num, buffer, 10);
        return;
    }

    // print the sign if the number is negative
	if(num < 0.0)
	{
		*buffer++ = '-';
		num = -num;
	}

	// round the number to the precision
	double rounding = 0.5;
	for(unsigned char i = 0; i < (precision + 1); ++i)
		rounding /= 10.0;
	num += rounding;

	// print the integer part
	unsigned long integer = (unsigned long)num;
	double remainder = num - (double)integer;

    // store the number of integers for fast reversing later
    unsigned int integers = 0;
    // loop until the integer is 0 at least once
    do
    {
        // add the first digit to the buffer
        *buffer++ = '0' + (integer % 10);
        integer /= 10;
        integers++;
    } while(integer > 0);

    // reverse the buffer to get the correct order
    this->reverse(buffer - integers, integers);

	// print the decimal point
	if(precision > 0)
		*buffer++ = '.';

	// print the decimal part
	while(precision-- > 0)
	{
		remainder *= 10.0;
		int digit = int(remainder);
        *buffer++ = '0' + digit;
		remainder -= digit;
        integers++;
	}
}

void Print::reverse(char* buffer, unsigned int length)
{
    for(int i = 0; i < length/2; i++) {
        char temp = *(buffer + i);
        *(buffer + i) = *(buffer + length - i - 1);
        *(buffer + length - i - 1) = temp;
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
void Print::drawAscii(const char character, unsigned int size)
{
    // get the relevant bitmap data which is indexed according to the ascii table
    const unsigned int* bitmap = FONT(character);

    // if the bitmap is a null pointer or overflows the frame buffer, return 0
    if (bitmap == nullptr || ((FONT_WIDTH * FONT_HEIGHT) * size > this->totalPixels))
        return;

    // minimum size is 1, not 0
    if (size == 0) size = 1;

    // get our current framebuffer pointer location
    unsigned int bufferPosition = this->cursor;
    // keep track of the current row position
    unsigned int rowPosition = 0;
    // calculate the row size
    unsigned int rowSize = FONT_WIDTH * size;

    // loop through the bitmap data
    for (int j = 0; j < FONT_DATA; j++)
    {
        // get the current data, and multiply it by the size to get the number of pixels to draw
        unsigned int data = bitmap[j] * size;
        // if the current data is 0, we have completed our loop
        if (data == 0) break;

        // loop through the data
        for (int i = 0; i < data; i++)
        {
            // if the index is odd, draw a pixel
            if (j & 0x1) this->frameBuffer[rowPosition + bufferPosition] = this->color;

            // increment the row position
            rowPosition++;

            // if the row position is equal to the row size, we have completed a row
            if (rowPosition >= rowSize)
            {
                rowPosition = 0;
                bufferPosition += this->params.width;
            }
        }
    }

    // set the cursor to the end of the character
    this->cursor += rowSize;
}