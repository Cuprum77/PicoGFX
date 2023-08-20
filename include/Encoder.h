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

// This has to be manually set, as the precompiler is a bit dumb
#define METADATA_BYTES 9
struct stream_metadata_t
{
	unsigned char type;			// 1 byte
	unsigned short width;		// 2 bytes
	unsigned short height;		// 2 bytes
	unsigned int totalBytes;	// 4 bytes
};

struct stream_config_t
{
	unsigned short monochromeCutoff;
	bool monochromeDithering;
	bool isReceiverBigEndian;
};

class Encoder
{
public:
	void Encode(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	unsigned int Decode(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);

	void AddMetadata(stream_metadata_t* metadata, unsigned char* stream);
	void StripMetadata(stream_metadata_t* metadata, unsigned char* stream);

private:
	void EncodeMonochrome(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeMonochromeRLE(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeRunLengthEncoding(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeLossy(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeReducedColor(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeReducedColorRLE(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);
	void EncodeRaw(stream_metadata_t* metadata, stream_config_t config, unsigned short* frameBuffer, unsigned char* stream);

	unsigned int DecodeMonochrome(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeMonochromeRLE(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeRunLengthEncoding(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeLossy(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeReducedColor(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeReducedColorRLE(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);
	unsigned int DecodeRaw(stream_metadata_t* metadata, unsigned char* stream, size_t streamSize, unsigned short* frameBuffer);

	void MonochromeDither(stream_metadata_t* metadata, unsigned short* frameBuffer, unsigned short strength);
};