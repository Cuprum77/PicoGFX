#pragma once

#include "Display.hpp"

class Print
{
public:
    Print(Display* display);

    void write(char c, uchar size = 1, uchar base = DEC);

    void print(char num, Color color, uchar size = 1, uchar base = DEC);
    void print(uchar c, uchar size = 1, uchar base = DEC);
    void print(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void print(short num, uchar size = 1, uchar base = DEC);
    void print(short num, Color color, uchar size = 1, uchar base = DEC);
    void print(ushort num, uchar size = 1, uchar base = DEC);
    void print(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void print(int num, uchar size = 1, uchar base = DEC);
    void print(int num, Color color, uchar size = 1, uchar base = DEC);
    void print(uint num, uchar size = 1, uchar base = DEC);
    void print(uint num, Color color, uchar size = 1, uchar base = DEC);
    void print(long num, uchar size = 1, uchar base = DEC);
    void print(long num, Color color, uchar size = 1, uchar base = DEC);
    void print(ulong num, uchar size = 1, uchar base = DEC);
    void print(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void print(double num, uint precision = 2, uchar size = 1);
    void print(double num, Color color, uint precision = 2, uchar size = 1);
    void print(const char* text, uchar size = 1);
    void print(const char* text, Color color, uchar size = 1);
    void print(bool value, uchar size = 1);

    void println(char c, uchar size = 1, uchar base = DEC);
    void println(char num, Color color, uchar size = 1, uchar base = DEC);
    void println(uchar c, uchar size = 1, uchar base = DEC);
    void println(uchar num, Color color, uchar size = 1, uchar base = DEC);
    void println(int num, uchar size = 1, uchar base = DEC);
    void println(int num, Color color, uchar size = 1, uchar base = DEC);
    void println(uint num, uchar size = 1, uchar base = DEC);
    void println(uint num, Color color, uchar size = 1, uchar base = DEC);
    void println(short num, uchar size = 1, uchar base = DEC);
    void println(short num, Color color, uchar size = 1, uchar base = DEC);
    void println(ushort num, uchar size = 1, uchar base = DEC);
    void println(ushort num, Color color, uchar size = 1, uchar base = DEC);
    void println(long num, uchar size = 1, uchar base = DEC);
    void println(long num, Color color, uchar size = 1, uchar base = DEC);
    void println(ulong num, uchar size = 1, uchar base = DEC);
    void println(ulong num, Color color, uchar size = 1, uchar base = DEC);
    void println(double num, uint precision = 2, uchar size = 1);
    void println(double num, Color color, uint precision = 2, uchar size = 1);
    void println(const char* text, uchar size = 1);
    void println(const char* text, Color color, uchar size = 1);
    void println(bool value, uchar size = 1);
    void println(void);

    uint getStringLength(char num, uchar size = 1, uchar base = DEC);
    uint getStringLength(uchar num, uchar size = 1, uchar base = DEC);
    uint getStringLength(int num, uchar size = 1, uchar base = DEC);
    uint getStringLength(uint num, uchar size = 1, uchar base = DEC);
    uint getStringLength(short num, uchar size = 1, uchar base = DEC);
    uint getStringLength(ushort num, uchar size = 1, uchar base = DEC);
    uint getStringLength(long num, uchar size = 1, uchar base = DEC);
    uint getStringLength(ulong num, uchar size = 1, uchar base = DEC);
    uint getStringLength(double num, uchar precision = 2, uchar size = 1);
    uint getStringLength(const char* text, uchar size = 1);
    uint getStringLength(bool value, uchar size = 1);
private:
    Display* display;

    uint drawAscii(const char c, Point Point, uint size, Color color);
    void floatToString(double num, char* buffer, uint precision);
    void reverse(char* str, uint length);
};