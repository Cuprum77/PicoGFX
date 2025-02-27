#include <compression_decoder.h>

uint32_t compression_decoder::decode(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		return this->decodeMonochrome(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::MONOCHROME_RLE:
		return this->decodeMonochromeRLE(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::RUN_LENGHT_ENCODING:
		return this->decodeRunLengthEncoding(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::LOSSY:
		return this->decodeLossy(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::REDUCED_COLOR:
		return this->decodeReducedColor(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::REDUCED_COLOR_RLE:
		return this->decodeReducedColorRLE(metadata, stream, streamSize, frameBuffer);
	case encoding_type_t::RAW:
		default:
		return this->decodeRaw(metadata, stream, streamSize, frameBuffer);
	}
}

uint32_t compression_decoder::decodeMonochrome(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	// Here we reverse what the monochrome encoder did
	uint32_t framebufferIndex = 0;

	// copy the framebuffer to the display
	for (int32_t i = 0; i < streamSize; i++)
	{
		// get each pixel
		uint8_t pixel = 0;
		pixel = stream[i];

		// copy each bit of the pixel to the output
		for(int32_t j = 0; j < 8; j++)
			frameBuffer[i * 8 + j] = (pixel & (0x1 << (7 - j))) ? 0xffff : 0x0000;

		framebufferIndex += 8;
	}

	return framebufferIndex;
}

uint32_t compression_decoder::decodeMonochromeRLE(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	uint32_t outputBufferIndex = 0;

	// loop through all the bytes
	for (int32_t streamIndex = 0; streamIndex < streamSize;)
	{
		// get the count and pixel value from the stream with an offset
		uint32_t count = stream[streamIndex] >> 0x01;
		uint8_t pixel = stream[streamIndex++] & 0x1;

		// reverse the run length encoding
		for (int32_t i = 0; i < count; i++)
			frameBuffer[outputBufferIndex++] = pixel ? 0xffff : 0x0000;
	}

	return outputBufferIndex;
}

uint32_t compression_decoder::decodeRunLengthEncoding(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	uint32_t outputBufferIndex = 0;

	for (int32_t streamIndex = 0; streamIndex < streamSize;)
	{
		// get the count and pixel value from the stream with an offset
		uint32_t count = stream[streamIndex++];
		uint16_t pixel = (stream[streamIndex++] << 0x8) | stream[streamIndex++];

		// reverse the run length encoding
		for (int32_t i = 0; i < count; i++)
			frameBuffer[outputBufferIndex++] = pixel;
	}

	return outputBufferIndex;
}

uint32_t compression_decoder::decodeLossy(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	int32_t r, g, b;
	int32_t y, cb, cr;
	int32_t pixelIndex = 0;
	int32_t lastY = 0, lastCB = 128, lastCR = 128; // Initialize chroma to mid-range.

	for (uint32_t streamIndex = 0; streamIndex < streamSize;) // Loop through all pixels in the frame
	{
		uint32_t count = stream[streamIndex++];
		y = stream[streamIndex++];
		cb = stream[streamIndex++];
		cr = stream[streamIndex++];

		for (uint32_t i = 0; i < count * 2; i++)
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

			uint16_t pixel = (r << 11) | (g << 5) | b;
			frameBuffer[pixelIndex++] = pixel;
		}
	}

	return pixelIndex;
}

uint32_t compression_decoder::decodeReducedColor(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	// Here we reverse what the reduced color encoder does
	uint32_t pixelIndex = 0;

	// loop through each pixel in the framebuffer
	for (; pixelIndex < streamSize; pixelIndex++)
	{
		// build up the pixel based on the stream buffer
		uint8_t pixel = stream[pixelIndex];

		// extract the rgb values
		uint8_t r = (pixel >> 5) & 0x7;
		uint8_t g = (pixel >> 2) & 0x7;
		uint8_t b = pixel & 0x3;

		// convert the pixel to RGB565
		r = (r << 0x2) | (r >> 0x1);
		g = (g << 0x3) | (g << 0x1) | (g >> 0x2);
		b = (b << 0x3) | (b << 0x1) | (b >> 0x1);

		// reassemble the pixel
		uint16_t newPixel = (r << 0xb) | (g << 0x5) | b;
		frameBuffer[pixelIndex] = newPixel;
	}

	return pixelIndex;
}

uint32_t compression_decoder::decodeReducedColorRLE(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	uint8_t pixel;
	uint8_t count;
	uint16_t newPixel;
	int32_t pixelIndex = 0;

	for (int32_t streamIndex = 0; streamIndex < streamSize;)
	{
		count = stream[streamIndex++];
		pixel = stream[streamIndex++];

		// extract the rgb values
		uint8_t r = (pixel >> 5) & 0x7;
		uint8_t g = (pixel >> 2) & 0x7;
		uint8_t b = pixel & 0x3;

		// convert the pixel to RGB565
		r = (r << 0x2) | (r >> 0x1);
		g = (g << 0x3) | (g << 0x1) | (g >> 0x2);
		b = (b << 0x3) | (b << 0x1) | (b >> 0x1);

		// reassemble the pixel
		newPixel = (r << 0xb) | (g << 0x5) | b;

		for (int32_t i = 0; i < count; i++)
			frameBuffer[pixelIndex++] = newPixel;
	}

	return pixelIndex;
}

uint32_t compression_decoder::decodeRaw(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer)
{
	// Here we reverse what the raw encoder does
	uint32_t pixelIndex = 0;

	// loop through each pixel in the framebuffer
	for (uint32_t streamIndex = 0; streamIndex < streamSize;)
		// build up the pixel based on the stream buffer
		frameBuffer[pixelIndex++] = (stream[streamIndex++] << 0x8) | stream[streamIndex++];

	return pixelIndex;
}