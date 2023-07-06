#include "Encoder.h"

Encoder::Encoder(SLIC* slic)
{
	this->slic = slic;
}

void Encoder::Encode(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// set the size to 0 so we are sure this isnt bad
	metadata->totalBytes = 0;

	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		this->EncodeMonochrome(metadata, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::LOSSY:
		this->EncodeLossy(metadata, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::LOSSLESS:
		this->EncodeLossless(metadata, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR:
		this->EncodeReducedColor(metadata, frameBuffer, outputBuffer);
		break;
	case encoding_type_t::RAW:
	default:
		metadata->type = encoding_type_t::RAW;
		this->EncodeRaw(metadata, frameBuffer, outputBuffer);
		break;
	}

	// add the metadata to the output
	this->AddMetadata(metadata, outputBuffer);
}

void Encoder::Decode(metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer)
{
	// fetch the metadata
	this->StripMetadata(metadata, stream);

	switch (metadata->type)
	{
	case encoding_type_t::MONOCHROME:
		this->DecodeMonochrome(metadata, stream, frameBuffer);
		break;
	case encoding_type_t::LOSSY:
		this->DecodeLossy(metadata, stream, frameBuffer);
		break;
	case encoding_type_t::LOSSLESS:
		this->DecodeLossless(metadata, stream, frameBuffer);
		break;
	case encoding_type_t::REDUCED_COLOR:
		this->DecodeReducedColor(metadata, stream, frameBuffer);
		break;
	case encoding_type_t::RAW:
		this->DecodeRaw(metadata, stream, frameBuffer);
		break;
	default:
		break;
	}
}

void Encoder::EncodeMonochrome(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// To encode in monochrome, we can simply store each color as a bit. While run length encoding would compress it further,
	// the gains wouldnt be as great as we are already under 10kB per frame on most LCDs we support!	

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
			unsigned char bit = pixel > metadata->monochromeCutoff ? 0x1 : 0x0;
			byte |= bit << (7 - j);
		}

		// add the compressed byte to the output
		outputBuffer[METADATA_BYTES + i] = byte;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void Encoder::EncodeLossy(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	
}

void Encoder::EncodeLossless(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// This encoding is lossless, and the code below is more or less taken from the example of the SLIC github

	// Initialize the SLIC encoder to output to a RAM buffer
	int rc = slic->init_encode_ram(metadata->width, metadata->height, metadata->colorDepth, NULL, outputBuffer, RAM_BUFFER_SIZE);
	
	for (int y = 0; y < metadata->width && rc == SLIC_SUCCESS; y++)
	{			
		rc = slic->encode((unsigned char*)frameBuffer + y, metadata->width);
	}

	// append the size to the metadata
	metadata->totalBytes += slic->get_output_size();
}

void Encoder::EncodeReducedColor(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// calculate the output this is the size of the display in pixels
	int calculatedSize = metadata->width * metadata->height;

	// loop through each pixel in the framebuffer
	for (int i = 0; i < calculatedSize; i++)
	{
		unsigned short pixel = frameBuffer[i];

		// extract the color components
		unsigned char r = (pixel >> 0xb) & 0x1f;
		unsigned char g = (pixel >> 0x6) & 0x3f;
		unsigned char b = pixel & 0x1f;

		// convert the RGB565 format to a RGB332
		r = (r >> 2) & 0x7;
		g = (g >> 3) & 0x7;
		b = (b >> 3) & 0x3;

		// reconstruct the pixel and append to the output type
		unsigned char newPixel = (r << 5) | (g << 2) | b;
		outputBuffer[METADATA_BYTES + i] = newPixel;
	}

	// place the calculated size into the output size variable
	metadata->totalBytes += calculatedSize;
}

void Encoder::EncodeRaw(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer)
{
	// This isnt an actual encoding style, we will simply pack the data in a more convinient package for the serial interface.
	
	// calculate the output this is the size of the display in pixels
	int calculatedSize = metadata->width * metadata->height;

	// loop through each pixel in the framebuffer
	for (int i = 0; i < calculatedSize; i++)
	{
		unsigned short pixel = frameBuffer[i];

		unsigned char highPixel = (pixel >> 0x8) & 0xff;
		unsigned char lowPixel = pixel & 0xff;

		outputBuffer[METADATA_BYTES + (i * 2 + 0)] = highPixel;
		outputBuffer[METADATA_BYTES + (i * 2 + 1)] = lowPixel;
	}

	// place the calculated size into the output size variable, but multiply by two to account for 2 bytes per pixel
	metadata->totalBytes += calculatedSize * 2;
}

void Encoder::AddMetadata(metadata_t* metadata, unsigned char* outputBuffer)
{
	// Add the metadata to the beginning of the outputBuffer
	outputBuffer[0] = metadata->type;
	outputBuffer[1] = (metadata->width >> 0x8) & 0xff;
	outputBuffer[2] = metadata->width & 0xff;
	outputBuffer[3] = (metadata->height >> 0x8) & 0xff;
	outputBuffer[4] = metadata->height & 0xff;
	outputBuffer[5] = (metadata->totalBytes >> 0x18) & 0xff;
	outputBuffer[6] = (metadata->totalBytes >> 0x10) & 0xff;
	outputBuffer[7] = (metadata->totalBytes >> 0x8) & 0xff;
	outputBuffer[8] = metadata->totalBytes & 0xff;

	// Add the metadata length to the output size
	metadata->totalBytes += 0x9;
}

void Encoder::DecodeMonochrome(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer)
{
	// Here we reverse what the monochrome encoder did
	
	// copy the framebuffer to the display
	for (int i = 0; i < metadata->totalBytes; i++)
	{
		// get each pixel
		unsigned char pixel = stream[METADATA_BYTES + i];
		// copy each bit of the pixel to the output
		outputBuffer[i * 8 + 0] = (pixel & 0x80) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 1] = (pixel & 0x40) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 2] = (pixel & 0x20) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 3] = (pixel & 0x10) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 4] = (pixel & 0x08) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 5] = (pixel & 0x04) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 6] = (pixel & 0x02) ? 0xFFFF : 0x0000;
		outputBuffer[i * 8 + 7] = (pixel & 0x01) ? 0xFFFF : 0x0000;
	}
}

void Encoder::DecodeLossy(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer)
{

}

void Encoder::DecodeLossless(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer)
{
	int rc = slic->init_decode_ram(stream + METADATA_BYTES, metadata->totalBytes - METADATA_BYTES, NULL);
	int offset = 0;
	
	for (int y = 0; y < slic->get_height() && rc == SLIC_SUCCESS; y++)
	{
		rc = slic->decode((uint8_t*)tempBuffer, slic->get_width());
		// byte reverse
		for (int i = 0; i < slic->get_width(); i++)
		{
			outputBuffer[i + offset++] = ((tempBuffer[i] & 0x0F) << 4) | ((tempBuffer[i] & 0xF0) >> 4);
		}
	}
}

void Encoder::DecodeReducedColor(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer)
{
	// Here we reverse what the reduced color encoder does
	
	// loop through each pixel in the framebuffer
	for (int i = 0; i < metadata->totalBytes; i++)
	{
		// build up the pixel based on the stream buffer
		unsigned char pixel = stream[METADATA_BYTES + i];
		
		// extract the rgb values
		unsigned char r = (pixel >> 5) & 0x7;
		unsigned char g = (pixel >> 2) & 0x7;
		unsigned char b = pixel & 0x3;

		// convert the pixel to RGB565
		r = (r << 0x2) | (r >> 0x1);
		g = (g << 0x3) | (g << 0x1) | (g >> 0x2);
		b = (b << 0x3) | (b << 0x1) | (b >> 0x1);

		// reassemble the pixel
		unsigned short newPixel = (r << 0xb) | (g << 0x6) | b;
		outputBuffer[i] = newPixel;
	}
}

void Encoder::DecodeRaw(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer)
{
	// Here we reverse what the raw encoder does

	// loop through each pixel in the framebuffer
	for (int i = 0; i < metadata->totalBytes; i++)
	{
		// build up the pixel based on the stream buffer
		outputBuffer[i] = (stream[METADATA_BYTES + (i * 2 + 0)] << 0x8) | stream[METADATA_BYTES + (i * 2 + 1)];
	}
}

void Encoder::StripMetadata(metadata_t* metadata, unsigned char* stream)
{
	metadata->type = stream[0];
	metadata->width = (stream[1] << 0x8) | stream[2];
	metadata->height = (stream[3] << 0x8) | stream[4];
	metadata->totalBytes = (stream[5] << 0x18) | (stream[6] << 0x10) | (stream[7] << 0x8) | stream[8];
}