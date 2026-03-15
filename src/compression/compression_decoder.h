#pragma once

#include <gfxmath.h>
#include <compression.h>

class compression_decoder : public compression
{
public:
	uint32_t decode(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);

private:
    uint32_t decodeMonochrome(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeMonochromeRLE(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeRunLengthEncoding(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeLossy(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeReducedColor(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeReducedColorRLE(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
	uint32_t decodeRaw(stream_metadata_t* metadata, uint8_t* stream, size_t streamSize, uint16_t* frameBuffer);
};