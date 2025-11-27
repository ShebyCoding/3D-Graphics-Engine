#pragma once

#include <string>
#include <iostream>
#include <glad.h>

#include "stb_image.h"

class Texture
{
private:
	GLuint _id = 0;
	GLenum _type = 0;

	int _width = 0;
	int _height = 0;

public:
	Texture(const std::string& texturePath, GLenum textureType = GL_TEXTURE_2D)
	{
		unsigned char* data = loadImageData(texturePath, _width, _height);

		if (!data)
		{
			return;
		}

		_type = textureType;

		// Create texture and bind it
		glGenTextures(1, &_id);
		glBindTexture(_type, _id);

		// Texture wrapping
		glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filtering
		glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Determine format
		//GLenum format = GL_RGB;
		//if (nrChannels == 1) format = GL_RED;
		//else if (nrChannels == 3) format = GL_RGB;
		//else if (nrChannels == 4) format = GL_RGBA;

		// Upload texture
		glTexImage2D(_type, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(_type);

		glBindTexture(_type, 0);

		stbi_image_free(data);
	}

	~Texture()
	{
		if (_id) glDeleteTextures(1, &_id);
	}

	GLuint getID() const { return _id; }

	GLenum getType() const { return _type; }

	void bind(const GLint textureUnit) const
	{
		if (_id && _type)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(_type, _id);
		}
	}

	void unbind() const
	{
		if (_type) glBindTexture(_type, 0);
	}

private:
	unsigned char* loadImageData(const std::string& texturePath, int& width, int& height) const
	{
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, NULL, STBI_rgb_alpha);

		if (!data)
		{
			std::cerr << "Failed to load texture: " << texturePath << "\n";
		}

		return data;
	}
};