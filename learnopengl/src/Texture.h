#pragma once

#include <stdint.h>

namespace TextureType
{
	enum TextureType {
		Diffuse = 0,
		Specular = 1
	};
}

struct color_t
{
	color_t(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	uint8_t r, g, b, a;
};

class Texture
{
public:
	Texture(const char* path, uint8_t textureType = TextureType::Diffuse, bool flipVertically = true);
	Texture(uint16_t _width, uint16_t _height, uint8_t textureType = TextureType::Diffuse);
	~Texture();

	void Flush();
	void GenMipmaps();

	void SetParameter(uint32_t parameter, int32_t value) const; // overload this for more types

	// there are atleast 16 texture units on most systems (TEXTURE0 to TEXTURE15)
	void Bind(uint8_t unit = 0) const;
	void Unbind(uint8_t unit = 0) const;
public:
	uint32_t ID;
	color_t* data;

	uint16_t width;
	uint16_t height;
	uint8_t numChannels;
	uint8_t type;
};