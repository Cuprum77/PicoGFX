#pragma once

#include <slic.h>

#define METADATA_BYTES 0x9 // Should be 9 bytes
#define RAM_BUFFER_SIZE 2048
#define TEMP_BUFFER_SIZE 320

enum encoding_type_t
{
	MONOCHROME,
	LOSSY,
	LOSSLESS,
	REDUCED_COLOR,
	RAW,
};

struct metadata_t
{
	unsigned char type;			        // 1 byte
	unsigned short width;		        // 2 bytes
	unsigned short height;		        // 2 bytes
	unsigned int totalBytes;	        // 4 bytes
	unsigned char colorDepth;			// Not included in file, only for encoding
    unsigned short monochromeCutoff;    // Not included in file, only for encoding
};

class Encoder
{
public:
	Encoder(SLIC* slic);

	void Encode(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void Decode(metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);

private:
	void EncodeMonochrome(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeLossy(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeLossless(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeReducedColor(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeRaw(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void AddMetadata(metadata_t* metadata, unsigned char* outputBuffer);

	void DecodeMonochrome(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeLossy(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeLossless(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeReducedColor(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeRaw(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void StripMetadata(metadata_t* metadata, unsigned char* stream);

	SLIC* slic;
	unsigned short tempBuffer[TEMP_BUFFER_SIZE];
};