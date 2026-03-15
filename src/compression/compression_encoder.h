#pragma once

#include <gfxmath.h>
#include <compression.h>

class compression_encoder : public compression
{
public:
	void encode(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);

private:
	void encodeMonochrome(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeMonochromeRLE(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeRunLengthEncoding(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeLossy(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeReducedColor(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeReducedColorRLE(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void encodeRaw(stream_metadata_t* metadata, stream_config_t config, uint16_t* frameBuffer, uint8_t* stream);
	void monochromeDither(stream_metadata_t* metadata, uint16_t* frameBuffer, uint16_t strength);
};