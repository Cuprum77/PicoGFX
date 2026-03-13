#include <compression.h>

void compression::addMetadata(stream_metadata_t *metadata, uint8_t *stream)
{
	// create an index to easily move data around if needed
	int32_t index = 0;

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

void compression::stripMetadata(stream_metadata_t *metadata, uint8_t *stream)
{
	// create an index to easily move data around if needed
	int32_t index = 0;

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