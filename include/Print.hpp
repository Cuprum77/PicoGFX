#pragma once

#include "Color.hpp"
#include "Structs.hpp"
#include <string.h>
#include "Font.h"

// String behavior
#define CHARACTER_BUFFER_SIZE 128
#define TAB_SIZE 4      // how many spaces a tab is worth
#define FALSE "false"   // string representation of false
#define TRUE "true"     // string representation of true

// Typedefs for number bases
typedef enum
{
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16
} number_base_t;

class Print
{
public:
    Print(unsigned short* frameBuffer, Display_Params params);
    void setColor(Color color) { this->color = color.to16bit(); }
    Color getColor(void) { return Color(this->color); }
    void setCursor(Point point) { this->cursor = (unsigned int)(point.x) + (unsigned int)(point.y * this->params.width); }
    Point getCursor(void) { return {(unsigned int)(this->cursor % this->params.width), (unsigned int)(this->cursor / this->params.width)}; }

    void write(char c, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->print((long)c, size, base); }
    void print(unsigned char c, unsigned char size = 1, number_base_t base = number_base_t::DEC)  { this->print((unsigned long)c, size, base); }
    void print(short num, unsigned char size = 1, number_base_t base = number_base_t::DEC)  { this->print((long)num, size, base); }
    void print(unsigned short num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->print((unsigned long)num, size, base); }
    void print(int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->print((long)num, size, base); }
    void print(unsigned int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->print((unsigned long)num, size, base); }
    void print(long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    void print(unsigned long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    void print(double num, unsigned char size = 1, unsigned int precision = 2);
    void print(const char* text, unsigned char size = 1);
    void print(bool value, unsigned char size = 1);

    void println(char c, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((long)c, size, base); }
    void println(unsigned char c, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((unsigned long)c, size, base); }
    void println(int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((long)num, size, base); }
    void println(unsigned int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((unsigned long)num, size, base); }
    void println(short num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((long)num, size, base); }
    void println(unsigned short num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { this->println((unsigned long)num, size, base); }
    void println(long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    void println(unsigned long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    void println(double num, unsigned char size = 1, unsigned int precision = 2);
    void println(const char* text, unsigned char size = 1);
    void println(bool value, unsigned char size = 1);
    void println(void);

    unsigned int getStringLength(char num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, size, base); }
    unsigned int getStringLength(unsigned char num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, size, base); }
    unsigned int getStringLength(int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, size, base); }
    unsigned int getStringLength(unsigned int num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, size, base); }
    unsigned int getStringLength(short num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((long)num, size, base); }
    unsigned int getStringLength(unsigned short num, unsigned char size = 1, number_base_t base = number_base_t::DEC) { return this->getStringLength((unsigned long)num, size, base); }
    unsigned int getStringLength(long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    unsigned int getStringLength(unsigned long num, unsigned char size = 1, number_base_t base = number_base_t::DEC);
    unsigned int getStringLength(double num, unsigned char precision = 2, unsigned char size = 1);
    unsigned int getStringLength(const char* text, unsigned char size = 1);
    unsigned int getStringLength(bool value, unsigned char size = 1);
private:
    unsigned short* frameBuffer;
    Display_Params params;
    size_t totalPixels;
    unsigned long cursor;
    unsigned short color;

    void drawAscii(const char c, unsigned int size);
    void floatToString(double num, char* buffer, unsigned int precision);
    void reverse(char* str, unsigned int length);
};