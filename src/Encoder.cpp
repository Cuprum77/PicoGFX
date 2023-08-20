#include "Encoder.h"
#include <stdio.h>

void Encoder::Encode(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// set the size to 0 so we are sure this isnt bad
	metadata->totalBytes = 0;

	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		this->EncodeMonochrome(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::MONOCHROME_RLE:
		this->EncodeMonochromeRLE(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::RUN_LENGHT_ENCODING:
		this->EncodeRunLengthEncoding(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::LOSSY:
		this->EncodeLossy(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR:
		this->EncodeReducedColor(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR_RLE:
		this->EncodeReducedColorRLE(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::RAW:
	default:
		metadata->type = encoding_type_t::RAW;
		this->EncodeRaw(metadata, config, frameBuffer, outputBuffer);
		break;
	}
}

unsigned int Encoder::Decode(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		return this->DecodeMonochrome(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::MONOCHROME_RLE:
		return this->DecodeMonochromeRLE(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::RUN_LENGHT_ENCODING:
		return this->DecodeRunLengthEncoding(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::LOSSY:
		return this->DecodeLossy(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::REDUCED_COLOR:
		return this->DecodeReducedColor(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::REDUCED_COLOR_RLE:
		return this->DecodeReducedColorRLE(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::RAW:
		default:
		return this->DecodeRaw(metadata, stream, streamSize, frameBuffer);
	}
}

void Encoder::AddMetadata(stream_metadata_t* metadata, unsigned char* stream)
{
	// create an index to easily move data around if needed
	int index = 0;

	// Add the metadata to the beginning of the stream
	stream[index++] = metadata->type;	// add type

	// Add the width and height to the stream buffer
	stream[index++] = (metadata->width >> 0x8) & 0xff;
	stream[index++] = metadata->width & 0xff;
	stream[index++] = (metadata->height >> 0x8) & 0xff;
	stream[index++] = metadata->height & 0xff;

	// Add the total bytes to the stream buffer
	stream[index++] = (metadata->totalBytes >> 0x18) & 0xff;
	stream[index++] = (metadata->totalBytes >> 0x10) & 0xff;
	stream[index++] = (metadata->totalBytes >> 0x8) & 0xff;
	stream[index++] = metadata->totalBytes & 0xff;
}

void Encoder::StripMetadata(stream_metadata_t* metadata, unsigned char* stream)
{
	// create an index to easily move data around if needed
	int index = 0;

	// add the type of stream and the rotation
	metadata->type = stream[index++];

	// add the width and height
	metadata->width = (stream[index++] << 0x8);
	metadata->width |= stream[index++];
	metadata->height = (stream[index++] << 0x8);
	metadata->height |= stream[index++];

	// add the total bytes
	metadata->totalBytes = (stream[index++] << 0x18);
	metadata->totalBytes |= (stream[index++] << 0x10);
	metadata->totalBytes |= (stream[index++] << 0x8);
	metadata->totalBytes |= stream[index++];
}


void Encoder::EncodeMonochrome(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	// To encode in monochrome, we can simply store each color as a bit. While run length encoding would compress it further,
	// the gains wouldnt be as great as we are already under 10kB per frame on most LCDs we support!	

	if (config.monochromeDithering)
		this->MonochromeDither(metadata, frameBuffer, config.monochromeCutoff);

	// calculate the output buffer this is the size of the display in pixels divided by bits in a byte
	int calculatedSize = (metadata->width * metadata->height) / 8;

	// loop through the precalculated amount of bytes
	for (int i = 0; i < calculatedSize; i++)
	{
		unsigned char byte = 0;

		// loop through 8 pixels to get their values
		for (int j = 0; j < 8; j++)
		{
			unsigned short pixel = frameBuffer[i * 8 + j];
			unsigned char bit = pixel > config.monochromeCutoff ? 0x1 : 0x0;
			byte |= bit << (7 - j);
		}

		// add the compressed byte to the output
		stream[METADATA_BYTES + i] = byte;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void Encoder::EncodeMonochromeRLE(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	if (config.monochromeDithering)
		this->MonochromeDither(metadata, frameBuffer, config.monochromeCutoff);

	int streamIndex = 0;
	unsigned int count = 1;
	unsigned char oldBit = 0, bit = 0;
	unsigned int maxSize = metadata->width * metadata->height;

	for (int pixelIndex = 0; pixelIndex < maxSize; pixelIndex++)
	{
		unsigned short pixelRaw = frameBuffer[pixelIndex];
		bit = pixelRaw > config.monochromeCutoff ? 0x01 : 0x00;

		// Perform run-length encoding
		if (bit != oldBit)
		{
			while (count > 0)
			{
				unsigned char thisCount = count > 127 ? 127 : count;
				stream[METADATA_BYTES + streamIndex++] = (unsigned char)((thisCount << 1) | oldBit);
				// decrement count by the amount we've just encoded
				count -= thisCount;
			}
			oldBit = bit;
			count = 1;
		}
		else
		{
			count++;
		}
	}

	// If the old bit still isnt set, set it. This should fix the single color bug
	if (bit != oldBit && count != 1)
		oldBit = bit;

	// Write the last pixel and count
	if (count > 1)
	{
		while (count > 0)
		{
			unsigned char thisCount = count > 127 ? 127 : count;
			stream[METADATA_BYTES + streamIndex++] = (unsigned char)((thisCount << 1) | oldBit);
			// decrement count by the amount we've just encoded
			count -= thisCount;
		}
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += streamIndex;
}

void Encoder::EncodeRunLengthEncoding(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	unsigned int streamIndex = 0, count = 1;
	unsigned short oldPixel = 0, pixel = 0;
	unsigned int maxSize = metadata->width * metadata->height;

	for (int pixelIndex = 0; pixelIndex < maxSize; pixelIndex++)
	{
		pixel = frameBuffer[pixelIndex];

		// Perform run-length encoding
		if (pixel != oldPixel)
		{
			while (count > 0)
			{
				unsigned char thisCount = count > 255 ? 255 : count;
				if (config.isReceiverBigEndian)
				{
					stream[METADATA_BYTES + streamIndex++] = thisCount;
					stream[METADATA_BYTES + streamIndex++] = (oldPixel >> 0x8) & 0xff;
					stream[METADATA_BYTES + streamIndex++] = oldPixel & 0xff;
				}
				else
				{
					stream[METADATA_BYTES + streamIndex++] = oldPixel & 0xff;
					stream[METADATA_BYTES + streamIndex++] = (oldPixel >> 0x8) & 0xff;
					stream[METADATA_BYTES + streamIndex++] = thisCount;
				}
				// decrement count by the amount we've just encoded
				count -= thisCount;
			}
			oldPixel = pixel;
			count = 1;
		}
		else
		{
			count++;
		}
	}

	// If the old bit still isnt set, set it. This should fix the single color bug
	if (pixel != oldPixel && count != 1)
		oldPixel = pixel;

	// Write the last pixel and count
	if (count > 1)
	{
		while (count > 0)
		{
			unsigned char thisCount = count > 255 ? 255 : count;
			if (config.isReceiverBigEndian)
			{
				stream[METADATA_BYTES + streamIndex++] = thisCount;
				stream[METADATA_BYTES + streamIndex++] = (oldPixel >> 0x8) & 0xff;
				stream[METADATA_BYTES + streamIndex++] = oldPixel & 0xff;
			}
			else
			{
				stream[METADATA_BYTES + streamIndex++] = oldPixel & 0xff;
				stream[METADATA_BYTES + streamIndex++] = (oldPixel >> 0x8) & 0xff;
				stream[METADATA_BYTES + streamIndex++] = thisCount;
			}
			// decrement count by the amount we've just encoded
			count -= thisCount;
		}
	}

	// Append the streamIndex to the metadata length
	metadata->totalBytes += streamIndex;
}

void Encoder::EncodeLossy(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	unsigned int r, g, b;
	unsigned int y, cb, cr;
	unsigned int oldY = 0, oldCB = 0, oldCR = 0;
	int pixelIndex = 0, streamIndex = 0;
	unsigned int count = 0;

	unsigned int framebufferSize = metadata->width * metadata->height;
	for (pixelIndex = 0; pixelIndex < framebufferSize; pixelIndex++) {
		unsigned short pixel = frameBuffer[pixelIndex];

		r = ((pixel >> 11) & 0x1F) * 8; // approximation of *255/31
		g = ((pixel >> 5) & 0x3F) * 4;  // approximation of *255/63
		b = (pixel & 0x1F) * 8; // approximation of *255/31

		// Integer approximation of conversion constants
		y = (77 * r + 150 * g + 29 * b) >> 8;
		cb = 128 + ((-44 * r - 87 * g + 131 * b) >> 8);
		cr = 128 + ((131 * r - 110 * g - 21 * b) >> 8);

		// Perform chroma subsampling (use every other pixel)
		if (pixelIndex % 2 == 0) {
			// Perform run-length encoding
			if (y != oldY || cb != oldCB || cr != oldCR)
			{
				while (count > 0)
				{
					unsigned char thisCount = count > 255 ? 255 : count;
					if (config.isReceiverBigEndian)
					{
						stream[METADATA_BYTES + streamIndex++] = thisCount;
						stream[METADATA_BYTES + streamIndex++] = oldY;
						stream[METADATA_BYTES + streamIndex++] = oldCB;
						stream[METADATA_BYTES + streamIndex++] = oldCR;
					}
					else
					{
						stream[METADATA_BYTES + streamIndex++] = oldCR;
						stream[METADATA_BYTES + streamIndex++] = oldCB;
						stream[METADATA_BYTES + streamIndex++] = oldY;
						stream[METADATA_BYTES + streamIndex++] = thisCount;
					}
					// decrement count by the amount we've just encoded
					count -= thisCount;
				}

				oldY = y;
				oldCB = cb;
				oldCR = cr;

				count = 1;
			}
			else
			{
				count++;
			}
		}
	}

	// Write the last pixel and count
	if (count > 0)
	{
		while (count > 0)
		{
			unsigned char thisCount = count > 255 ? 255 : count;
			if (config.isReceiverBigEndian)
			{
				stream[METADATA_BYTES + streamIndex++] = thisCount;
				stream[METADATA_BYTES + streamIndex++] = oldY;
				stream[METADATA_BYTES + streamIndex++] = oldCB;
				stream[METADATA_BYTES + streamIndex++] = oldCR;
			}
			else
			{
				stream[METADATA_BYTES + streamIndex++] = oldCR;
				stream[METADATA_BYTES + streamIndex++] = oldCB;
				stream[METADATA_BYTES + streamIndex++] = oldY;
				stream[METADATA_BYTES + streamIndex++] = thisCount;
			}
			// decrement count by the amount we've just encoded
			count -= thisCount;
		}
	}

	// Append the streamIndex to the metadata length
	metadata->totalBytes += streamIndex;
}

void Encoder::EncodeReducedColor(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	// calculate the output this is the size of the display in pixels
	int calculatedSize = metadata->width * metadata->height;

	// loop through each pixel in the framebuffer
	for (int i = 0; i < calculatedSize; i++)
	{
		unsigned short pixel = frameBuffer[i];

		// extract the color components
		unsigned char r = (pixel >> 0xb) & 0x1f;
		unsigned char g = (pixel >> 0x5) & 0x3f;
		unsigned char b = pixel & 0x1f;

		// convert the RGB565 format to a RGB332
		r = (r >> 2) & 0x7;
		g = (g >> 3) & 0x7;
		b = (b >> 3) & 0x3;

		// reconstruct the pixel and append to the output type
		unsigned char newPixel = (r << 5) | (g << 2) | b;
		stream[METADATA_BYTES + i] = newPixel;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void Encoder::EncodeReducedColorRLE(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	int pixelIndex = 0, streamIndex = 0;
	unsigned int count = 0;
	unsigned char oldPixel = 0, newPixel = 0;

	for (pixelIndex = 0; pixelIndex < metadata->width * metadata->height; pixelIndex++)
	{
		unsigned short pixel = frameBuffer[pixelIndex];

		// extract the color components
		unsigned char r = (pixel >> 0xb) & 0x1f;
		unsigned char g = (pixel >> 0x5) & 0x3f;
		unsigned char b = pixel & 0x1f;

		// convert the RGB565 format to a RGB332
		r = (r >> 2) & 0x7;
		g = (g >> 3) & 0x7;
		b = (b >> 3) & 0x3;

		// reconstruct the pixel and append to the output type
		newPixel = (r << 5) | (g << 2) | b;

		// Perform run-length encoding
		if (newPixel != oldPixel)
		{
			while (count > 0)
			{
				unsigned char thisCount = count > 255 ? 255 : count;
				if (config.isReceiverBigEndian)
				{
					stream[METADATA_BYTES + streamIndex++] = count;
					stream[METADATA_BYTES + streamIndex++] = oldPixel;
				}
				else
				{
					stream[METADATA_BYTES + streamIndex++] = oldPixel;
					stream[METADATA_BYTES + streamIndex++] = count;
				}
				// decrement count by the amount we've just encoded
				count -= thisCount;
			}
			oldPixel = newPixel;
			count = 1;
		}
		else
		{
			count++;
		}
	}

	// If the old bit still isnt set, set it. This should fix the single color bug
	if (newPixel != oldPixel && count != 1)
		oldPixel = newPixel;

	// Write the last pixel and count
	if (count > 1)
	{
		while (count > 0)
		{
			unsigned char thisCount = count > 255 ? 255 : count;
			if (config.isReceiverBigEndian)
			{
				stream[METADATA_BYTES + streamIndex++] = count;
				stream[METADATA_BYTES + streamIndex++] = oldPixel;
			}
			else
			{
				stream[METADATA_BYTES + streamIndex++] = oldPixel;
				stream[METADATA_BYTES + streamIndex++] = count;
			}
			// decrement count by the amount we've just encoded
			count -= thisCount;
		}
	}

	// Append the streamIndex to the metadata length
	metadata->totalBytes += streamIndex;
}

void Encoder::EncodeRaw(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream)
{
	// This isnt an actual encoding style, we will simply pack the data in a more convinient package for the serial interface.

	// calculate the output this is the size of the display in pixels
	int calculatedSize = metadata->width * metadata->height;
	unsigned int streamIndex = 0;

	// loop through each pixel in the framebuffer
	for (int pixelIndex = 0; pixelIndex < calculatedSize;)
	{
		unsigned short pixel = frameBuffer[pixelIndex++];

		if (config.isReceiverBigEndian)
		{
			stream[METADATA_BYTES + streamIndex++] = (pixel >> 0x8) & 0xff;
			stream[METADATA_BYTES + streamIndex++] = pixel & 0xff;
		}
		else
		{
			stream[METADATA_BYTES + streamIndex++] = pixel & 0xff;
			stream[METADATA_BYTES + streamIndex++] = (pixel >> 0x8) & 0xff;
		}
	}

	// place the calculated size into the output size variable, but multiply by two to account for 2 bytes per pixel
	metadata->totalBytes += calculatedSize * 2;
}


unsigned int Encoder::DecodeMonochrome(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	// Here we reverse what the monochrome encoder did
	unsigned int framebufferIndex = 0;

	// copy the framebuffer to the display
	for (int i = 0; i < streamSize; i++)
	{
		// get each pixel
		unsigned char pixel = 0;
		pixel = stream[i];

		// copy each bit of the pixel to the output
		for(int j = 0; j < 8; j++)
			frameBuffer[i * 8 + j] = (pixel & (0x1 << (7 - j))) ? 0xffff : 0x0000;

		framebufferIndex += 8;
	}

	return framebufferIndex;
}

unsigned int Encoder::DecodeMonochromeRLE(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	unsigned int outputBufferIndex = 0;

	// loop through all the bytes
	for (int streamIndex = 0; streamIndex < streamSize;)
	{
		// get the count and pixel value from the stream with an offset
		unsigned int count = stream[streamIndex] >> 0x01;
		unsigned char pixel = stream[streamIndex++] & 0x1;

		// reverse the run length encoding
		for (int i = 0; i < count; i++)
			frameBuffer[outputBufferIndex++] = pixel ? 0xffff : 0x0000;
	}

	return outputBufferIndex;
}

unsigned int Encoder::DecodeRunLengthEncoding(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	unsigned int outputBufferIndex = 0;

	for (int streamIndex = 0; streamIndex < streamSize;)
	{
		// get the count and pixel value from the stream with an offset
		unsigned int count = stream[streamIndex++];
		unsigned short pixel = (stream[streamIndex++] << 0x8) | stream[streamIndex++];

		// reverse the run length encoding
		for (int i = 0; i < count; i++)
			frameBuffer[outputBufferIndex++] = pixel;
	}

	return outputBufferIndex;
}

unsigned int Encoder::DecodeLossy(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	int r, g, b;
	int y, cb, cr;
	int pixelIndex = 0;
	int lastY = 0, lastCB = 128, lastCR = 128; // Initialize chroma to mid-range.

	for (unsigned int streamIndex = 0; streamIndex < streamSize;) // Loop through all pixels in the frame
	{
		unsigned int count = stream[streamIndex++];
		y = stream[streamIndex++];
		cb = stream[streamIndex++];
		cr = stream[streamIndex++];

		for (unsigned int i = 0; i < count * 2; i++)
		{
			// Duplicate the chroma values for every other pixel
			if (i % 2 == 0)
			{
				lastY = y;
				lastCB = cb;
				lastCR = cr;
			}
			else
			{
				y = lastY;
				cb = lastCB;
				cr = lastCR;
			}

			// Integer approximation of conversion constants with scaling by 1024
			// To perform the operation we shift right by 10 (2^10 = 1024)
			r = y + ((cr - 128) * 1436 >> 10);
			g = y - ((cb - 128) * 352 >> 10) - ((cr - 128) * 731 >> 10);
			b = y + ((cb - 128) * 1815 >> 10);

			r = imax(0, imin(255, r));
			g = imax(0, imin(255, g));
			b = imax(0, imin(255, b));

			// Scale to the 565 color scheme
			r = (r >> 3) & 0x1F;
			g = (g >> 2) & 0x3F;
			b = (b >> 3) & 0x1F;

			unsigned short pixel = (r << 11) | (g << 5) | b;
			frameBuffer[pixelIndex++] = pixel;
		}
	}

	return pixelIndex;
}

unsigned int Encoder::DecodeReducedColor(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	// Here we reverse what the reduced color encoder does
	unsigned int pixelIndex = 0;

	// loop through each pixel in the framebuffer
	for (; pixelIndex < streamSize; pixelIndex++)
	{
		// build up the pixel based on the stream buffer
		unsigned char pixel = stream[pixelIndex];

		// extract the rgb values
		unsigned char r = (pixel >> 5) & 0x7;
		unsigned char g = (pixel >> 2) & 0x7;
		unsigned char b = pixel & 0x3;

		// convert the pixel to RGB565
		r = (r << 0x2) | (r >> 0x1);
		g = (g << 0x3) | (g << 0x1) | (g >> 0x2);
		b = (b << 0x3) | (b << 0x1) | (b >> 0x1);

		// reassemble the pixel
		unsigned short newPixel = (r << 0xb) | (g << 0x5) | b;
		frameBuffer[pixelIndex] = newPixel;
	}

	return pixelIndex;
}

unsigned int Encoder::DecodeReducedColorRLE(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	unsigned char pixel;
	unsigned char count;
	unsigned short newPixel;
	int pixelIndex = 0;

	for (int streamIndex = 0; streamIndex < streamSize;)
	{
		count = stream[streamIndex++];
		pixel = stream[streamIndex++];

		// extract the rgb values
		unsigned char r = (pixel >> 5) & 0x7;
		unsigned char g = (pixel >> 2) & 0x7;
		unsigned char b = pixel & 0x3;

		// convert the pixel to RGB565
		r = (r << 0x2) | (r >> 0x1);
		g = (g << 0x3) | (g << 0x1) | (g >> 0x2);
		b = (b << 0x3) | (b << 0x1) | (b >> 0x1);

		// reassemble the pixel
		newPixel = (r << 0xb) | (g << 0x5) | b;

		for (int i = 0; i < count; i++)
			frameBuffer[pixelIndex++] = newPixel;
	}

	return pixelIndex;
}

unsigned int Encoder::DecodeRaw(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer)
{
	// Here we reverse what the raw encoder does
	unsigned int pixelIndex = 0;

	// loop through each pixel in the framebuffer
	for (unsigned int streamIndex = 0; streamIndex < streamSize;)
		// build up the pixel based on the stream buffer
		frameBuffer[pixelIndex++] = (stream[streamIndex++] << 0x8) | stream[streamIndex++];

	return pixelIndex;
}


void Encoder::MonochromeDither(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned short strength)
{
	for (int y = 0; y < metadata->height; ++y) 
	{
		for (int x = 0; x < metadata->width; ++x) 
		{
			unsigned short oldPixel = frameBuffer[y * metadata->width + x];
			unsigned short newPixel = oldPixel > strength ? 0xffff : 0x0000;
			frameBuffer[y * metadata->width + x] = newPixel;

			int quant_error = oldPixel - newPixel;

			// Floyd-Steinberg dithering distributes error to neighboring pixels.
			if (x + 1 < metadata->width)
				frameBuffer[y * metadata->width + x + 1] += imin(imax(quant_error * 7 >> 4, 0), 0xffff);
			if (y + 1 < metadata->height) 
			{
				if (x - 1 >= 0)
					frameBuffer[(y + 1) * metadata->width + x - 1] += imin(imax(quant_error * 3 >> 4, 0), 0xffff);

				frameBuffer[(y + 1) * metadata->width + x] += imin(imax(quant_error * 5 >> 4, 0), 0xffff);

				if (x + 1 < metadata->width)
					frameBuffer[(y + 1) * metadata->width + x + 1] += imin(imax(quant_error * 1 >> 4, 0), 0xffff);
			}
		}
	}
}
