#pragma once

#include <GfxMath.h>

enum encoding_type_t
{
	MONOCHROME,
	MONOCHROME_RLE,
	RUN_LENGHT_ENCODING,
	LOSSY,
	REDUCED_COLOR,
	REDUCED_COLOR_RLE,
	RAW,
};

#define METADATA_BYTES sizeof(stream_metadata_t)
struct stream_metadata_t
{
	unsigned char type;			// 1 byte
	unsigned char rotation;		// 1 byte
	unsigned short width;		// 2 bytes
	unsigned short height;		// 2 bytes
	unsigned short chunkSize;	// 2 bytes
	unsigned int totalBytes;	// 4 bytes
};

struct stream_config_t
{
	unsigned short monochromeCutoff;
	bool monochromeDithering;
};

class Encoder
{
public:
	void Encode(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void Decode(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);

	void AddMetadata(stream_metadata_t* metadata, unsigned char* stream);
	void StripMetadata(stream_metadata_t* metadata, unsigned char* stream);

private:
	void EncodeMonochrome(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeMonochromeRLE(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeRunLengthEncoding(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeLossy(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeReducedColor(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeReducedColorRLE(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeRaw(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned char* stream);

	void DecodeMonochrome(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeMonochromeRLE(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeRunLengthEncoding(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeLossy(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeReducedColor(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeReducedColorRLE(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);
	void DecodeRaw(stream_metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);

	void MonochromeDither(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned short strength);
};