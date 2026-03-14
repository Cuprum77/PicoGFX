from PIL import Image, ImageFont, ImageDraw
import numpy as np
import string
from textwrap import wrap
import os
import argparse

parser = argparse.ArgumentParser(description="Generate font bitmap header files")
parser.add_argument("fonts", nargs="+", help="Font file(s) e.g. RobotoMono.ttf")
parser.add_argument("--sizes", nargs="+", type=int, default=[16, 20, 22, 24, 28, 32, 36, 48, 72],
                    help="Font size(s) e.g. --sizes 16 32 48")
parser.add_argument("--output", default="../src/print/fonts/", help="Output directory")
parser.add_argument("--font-dir", default=".", help="Directory to look for font files")
args = parser.parse_args()

font_file = args.fonts
font_size = args.sizes
output_dir = args.output
font_dir = args.font_dir

total_memory_usage = 0

# Function that takes the bad string.printables list and returns it in proper Ascii order
def GetAscii():
    Ascii = []

    # Add all the characters in order
    for i in range(32, 127):
        Ascii.append(chr(i))

    return Ascii


# Function that converts a character to a bitmap
def Convert(char, size, font):
    # Get the bounding box of the character
    char_size = font.getbbox(char)

    # Get all the dimensions
    width = char_size[2] - char_size[0]
    height = char_size[3] - char_size[1]
    x_offset = char_size[0]
    y_offset = char_size[1]

    # Create an image of the appropriate size
    image = Image.new('1', (width, height), color=0)
    draw = ImageDraw.Draw(image)

    # Draw the text onto the image
    draw.text((-x_offset, -y_offset), char, font=font, fill=1)

    # Convert the image to a numpy array and flatten
    bitmap = np.array(image).flatten()

    return bitmap, width, height, x_offset, y_offset


# Function to compress the data
def Compress(data):
    # We will compress it by only yielding the number of 0s and 1s after each other
    blacks = 0
    whites = 0

    for i, pixel in enumerate(data):
        if pixel == 1:
            whites += 1
        else:
            blacks += 1

        if data[i - 1] == pixel:
            continue

        if pixel == 1:
            yield blacks
            blacks = 0
        else:
            yield whites
            whites = 0

    # Handle the last pixel
    if whites != 0:
        yield whites
    elif blacks != 0:
        yield blacks


# Write out the character as a comment if it is not a regular character
def CharComment(char):
    if char == ' ':
        return "Character Space"
    elif char == '\\':
        return "Character Backslash"
    elif char == '\n':
        return "Character Newline"
    elif char == '\r':
        return "Character Carriage Return"
    elif char == '\t':
        return "Character Tab"
    else:
        return "Character {}".format(char)


# Draw the symbols as a sanity check
def Draw_Symbol(data, width):
    # Keep track of the row
    out = ""

    # Get each index of the list
    for i in range(len(data)):
        dataWidth = data[i]
        # If the index is odd, its colored
        if i % 2 == 0:
            # Print out the black characters as #
            out = out + "#" * dataWidth
        # If the index is even, its white
        else:
            # Print out the white characters as spaces
            out = out + "." * dataWidth

    # Add a newline after every width characters
    out = wrap(out, width)

    # Print the output
    for row in out:
        print(row)

def Generate_File(font_file, size):
    global total_memory_usage
    font_path = os.path.join(font_dir, font_file) if font_dir else font_file
    font = ImageFont.truetype(font_path, size)

    font_name = font_file[:-4]
    output_file = f"{font_name}{size}"

    # Get all Ascii characters
    AsciiList = GetAscii()

    # Generate all the ascii characters
    Ascii = []
    Details = []
    Pointer = []
    for char in AsciiList:
        # Ignore Ascii values below 33
        if ord(char) < 32:
            continue

        # Convert character to bitmap
        bitmap, width, height, x_offset, y_offset = Convert(char, size, font)
        bitmap = list(Compress(bitmap))
        Ascii.append(bitmap)
        Details.append((width, height, x_offset, y_offset))
        # Sanity check
        #print(f"{char}:")
        #Draw_Symbol(bitmap, width)

    # Calculate the total memory usage by getting the total elements in the matrix Ascii
    size_of_font = 0
    for i in range(len(Ascii)):
        size_of_font += len(Ascii[i])
    total_memory_usage += size_of_font

    # Output bitmap as C header file
    with open(os.path.abspath(output_dir) + '/' + output_file + ".font", "w") as f:
        header = f"""#pragma once

// Include the font struct for storing the font data
#include "fontstruct.h"

// Estimated memory usage: {str(size_of_font).replace(",", " ")} bytes

// Font bitmap data
static const uint32_t {font_name}{size}_bitmap[] = {{
    """
        f.write(header)

        # Convert each letter to a bitmap
        index = 0
        cnt = 0
        for char in AsciiList:
            # Ignore Ascii values below 32
            if ord(char) < 32:
                continue

            # Loop through each pixel and write to file
            ptr = 0
            for i in range(len(Ascii[index])):
                # Convert to Octal
                f.write("0x{:02x},".format(Ascii[index][i]))
                # Add a newline after every 16 characters
                if (cnt + 1) % 15 == 0 and cnt != 0:
                    f.write("\n\t")

                ptr += 1
                cnt += 1
            
            if(index != 0):
                Pointer.append(ptr + Pointer[index - 1])
            else:
                Pointer.append(ptr)
            index += 1
        f.write("\n};\n")

        detail_index = f"""
// Font details
static const FontCharacter {font_name}{size}_character[] = {{
"""
        f.write(detail_index)
        index = 0
        for detail in Details:
            width, height, x_offset, y_offset = detail
            # Get the location of the character on the bitmap
            sizePtr = Pointer[index]
            # Get the end of the character on the bitmap
            ptr = sizePtr - len(Ascii[index])
            f.write(f"\t{{ {ptr}, {sizePtr}, {width}, {height}, {x_offset}, {y_offset} }}")
            if index != len(Details) - 1:
                f.write(",")
            f.write("\t// ")
            f.write(CharComment(AsciiList[index]))
            f.write("\n")
            index += 1
        f.write("};\n")

        struct = f"""
// Font struct
inline FontStruct {font_name}{size} = {{
    .bitmap = {font_name}{size}_bitmap,
    .characters = {font_name}{size}_character,
    .size = {size_of_font},
    .newLineDistance = {size}
}};"""
        
        # Output struct for storing the font data
        f.write(struct)

def Generate_Header():
    global total_memory_usage

    # Output bitmap as C header file
    with open(os.path.abspath(output_dir) + '/' + "fontstruct.h", "w") as f:
        header = f"""#pragma once

#include <stdint.h>

// Estimated total memory usage: {total_memory_usage} bytes

// Struct for storing the location of the bitmap in memory
// Offset is the number of pixels from the upper left corner
typedef struct {{
    uint32_t pointer;
    uint32_t length;
    uint8_t width;
    uint8_t height;
    int8_t xOffset;
    int8_t yOffset;
}} FontCharacter;


// Struct for storing the font data
typedef struct {{
    const uint32_t *bitmap;
    const FontCharacter *characters;
    uint32_t size;
    uint32_t newLineDistance;
}} FontStruct;
"""
        f.write(header)


# Loop through each font size and generate a file
for font in font_file:
    for size in font_size:
        Generate_File(font, size)

# Generate the header file
Generate_Header()
print(f"Total memory usage: {str(total_memory_usage).replace(',', ' ')} bytes")
