#pragma once

#include <stdint.h>

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
	uint8_t type;			// 1 byte
	uint16_t width;			// 2 bytes
	uint16_t height;		// 2 bytes
	uint32_t totalBytes;	// 4 bytes
};

struct stream_config_t
{
	uint16_t monochromeCutoff;
	bool monochromeDithering;
	bool isReceiverBigEndian;
};

class compression
{
public:
    void addMetadata(stream_metadata_t* metadata, uint8_t* stream);
	void stripMetadata(stream_metadata_t* metadata, uint8_t* stream);
};