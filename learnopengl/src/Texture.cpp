#include "pch.h"
#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <string.h>

Texture::Texture(const char* path, uint8_t textureType, bool flipVertically)
{
	type = textureType;
	stbi_set_flip_vertically_on_load(flipVertically);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		int32_t tempWidth, tempHeight, tempNumChannels;
		data = stbi_load(path, &tempWidth, &tempHeight, &tempNumChannels, 4);
		if (!data)
		{
			char* str = (char*)malloc(5 + strlen(path));
			if (str)
			{
				strcpy(str, "res/");
				data = stbi_load(strcat(str, path), &tempWidth, &tempHeight, &tempNumChannels, 4);
				free(str);
			}
		}
		width = tempWidth, height = tempHeight, numChannels = tempNumChannels;
	}

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		perror(path), exit(1);
}

Texture::~Texture()
{
	stbi_image_free(data);
	glDeleteTextures(1, &ID);
}

void Texture::Update()
{
	glBindTexture(GL_TEXTURE_2D, ID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // glTexImage recreates the entire texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data); // glTexSubImage only updates the pixel data
	glGenerateMipmap(GL_TEXTURE_2D); // temporary - this is slow i think
}

void Texture::SetParameter(uint32_t parameter, int32_t value) const
{
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, parameter, value);
}

void Texture::Bind(uint8_t unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind(uint8_t unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}