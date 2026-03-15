#include <compression_encoder.h>
#include <stdio.h>

void compression_encoder::encode(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* outputBuffer)
{
	// set the size to 0 so we are sure this isnt bad
	metadata->totalBytes = 0;

	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		this->encodeMonochrome(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::MONOCHROME_RLE:
		this->encodeMonochromeRLE(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::RUN_LENGHT_ENCODING:
		this->encodeRunLengthEncoding(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::LOSSY:
		this->encodeLossy(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR:
		this->encodeReducedColor(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR_RLE:
		this->encodeReducedColorRLE(metadata, config, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::RAW:
	default:
		metadata->type = encoding_type_t::RAW;
		this->encodeRaw(metadata, config, frameBuffer, outputBuffer);
		break;
	}
}

void compression_encoder::encodeMonochrome(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	// To encode in monochrome, we can simply store each color as a bit. While run length encoding would compress it further,
	// the gains wouldnt be as great as we are already under 10kB per frame on most LCDs we support!	

	if (config.monochromeDithering)
		this->monochromeDither(metadata, frameBuffer, config.monochromeCutoff);

	// calculate the output buffer this is the size of the display in pixels divided by bits in a byte
	int32_t calculatedSize = (metadata->width * metadata->height) / 8;

	// loop through the precalculated amount of bytes
	for (int32_t i = 0; i < calculatedSize; i++)
	{
		uint8_t byte = 0;

		// loop through 8 pixels to get their values
		for (int32_t j = 0; j < 8; j++)
		{
			uint16_t pixel = frameBuffer[i * 8 + j];
			uint8_t bit = pixel > config.monochromeCutoff ? 0x1 : 0x0;
			byte |= bit << (7 - j);
		}

		// add the compressed byte to the output
		stream[METADATA_BYTES + i] = byte;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void compression_encoder::encodeMonochromeRLE(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	if (config.monochromeDithering)
		this->monochromeDither(metadata, frameBuffer, config.monochromeCutoff);

	int32_t streamIndex = 0;
	uint32_t count = 1;
	uint8_t oldBit = 0, bit = 0;
	uint32_t maxSize = metadata->width * metadata->height;

	for (int32_t pixelIndex = 0; pixelIndex < maxSize; pixelIndex++)
	{
		uint16_t pixelRaw = frameBuffer[pixelIndex];
		bit = pixelRaw > config.monochromeCutoff ? 0x01 : 0x00;

		// Perform run-length encoding
		if (bit != oldBit)
		{
			while (count > 0)
			{
				uint8_t thisCount = count > 127 ? 127 : count;
				stream[METADATA_BYTES + streamIndex++] = (uint8_t)((thisCount << 1) | oldBit);
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
			uint8_t thisCount = count > 127 ? 127 : count;
			stream[METADATA_BYTES + streamIndex++] = (uint8_t)((thisCount << 1) | oldBit);
			// decrement count by the amount we've just encoded
			count -= thisCount;
		}
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += streamIndex;
}

void compression_encoder::encodeRunLengthEncoding(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	uint32_t streamIndex = 0, count = 1;
	uint16_t oldPixel = 0, pixel = 0;
	uint32_t maxSize = metadata->width * metadata->height;

	for (int32_t pixelIndex = 0; pixelIndex < maxSize; pixelIndex++)
	{
		pixel = frameBuffer[pixelIndex];

		// Perform run-length encoding
		if (pixel != oldPixel)
		{
			while (count > 0)
			{
				uint8_t thisCount = count > 255 ? 255 : count;
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
			uint8_t thisCount = count > 255 ? 255 : count;
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

void compression_encoder::encodeLossy(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	uint32_t r, g, b;
	uint32_t y, cb, cr;
	uint32_t oldY = 0, oldCB = 0, oldCR = 0;
	int32_t pixelIndex = 0, streamIndex = 0;
	uint32_t count = 0;

	uint32_t framebufferSize = metadata->width * metadata->height;
	for (pixelIndex = 0; pixelIndex < framebufferSize; pixelIndex++) {
		uint16_t pixel = frameBuffer[pixelIndex];

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
					uint8_t thisCount = count > 255 ? 255 : count;
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
			uint8_t thisCount = count > 255 ? 255 : count;
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

void compression_encoder::encodeReducedColor(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	// calculate the output this is the size of the display in pixels
	int32_t calculatedSize = metadata->width * metadata->height;

	// loop through each pixel in the framebuffer
	for (int32_t i = 0; i < calculatedSize; i++)
	{
		uint16_t pixel = frameBuffer[i];

		// extract the color components
		uint8_t r = (pixel >> 0xb) & 0x1f;
		uint8_t g = (pixel >> 0x5) & 0x3f;
		uint8_t b = pixel & 0x1f;

		// convert the RGB565 format to a RGB332
		r = (r >> 2) & 0x7;
		g = (g >> 3) & 0x7;
		b = (b >> 3) & 0x3;

		// reconstruct the pixel and append to the output type
		uint8_t newPixel = (r << 5) | (g << 2) | b;
		stream[METADATA_BYTES + i] = newPixel;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void compression_encoder::encodeReducedColorRLE(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	int32_t pixelIndex = 0, streamIndex = 0;
	uint32_t count = 0;
	uint8_t oldPixel = 0, newPixel = 0;

	for (pixelIndex = 0; pixelIndex < metadata->width * metadata->height; pixelIndex++)
	{
		uint16_t pixel = frameBuffer[pixelIndex];

		// extract the color components
		uint8_t r = (pixel >> 0xb) & 0x1f;
		uint8_t g = (pixel >> 0x5) & 0x3f;
		uint8_t b = pixel & 0x1f;

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
				uint8_t thisCount = count > 255 ? 255 : count;
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
			uint8_t thisCount = count > 255 ? 255 : count;
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

void compression_encoder::encodeRaw(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream)
{
	// This isnt an actual encoding style, we will simply pack the data in a more convinient package for the serial interface.

	// calculate the output this is the size of the display in pixels
	int32_t calculatedSize = metadata->width * metadata->height;
	uint32_t streamIndex = 0;

	// loop through each pixel in the framebuffer
	for (int32_t pixelIndex = 0; pixelIndex < calculatedSize;)
	{
		uint16_t pixel = frameBuffer[pixelIndex++];

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

void compression_encoder::monochromeDither(stream_metadata_t* metadata, uint16_t* frameBuffer, uint16_t strength)
{
	for (int32_t y = 0; y < metadata->height; ++y) 
	{
		for (int32_t x = 0; x < metadata->width; ++x) 
		{
			uint16_t oldPixel = frameBuffer[y * metadata->width + x];
			uint16_t newPixel = oldPixel > strength ? 0xffff : 0x0000;
			frameBuffer[y * metadata->width + x] = newPixel;

			int32_t quant_error = oldPixel - newPixel;

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
