#pragma once

#include <stdint.h>

class Texture
{
public:
	Texture(const char* path, bool flipVertically = true);
	~Texture();

	void Update();

	void SetParameter(uint32_t parameter, int32_t value) const; // overload this for more types

	// there are atleast 16 texture units on most systems (TEXTURE0 to TEXTURE15)
	void Bind(uint8_t unit = 0) const;
	void Unbind(uint8_t unit = 0) const;
public:
	uint32_t ID;
	uint8_t* data;

	uint16_t width;
	uint16_t height;
	uint8_t numChannels;
};