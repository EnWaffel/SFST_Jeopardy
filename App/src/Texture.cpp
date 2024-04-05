#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

Texture::Texture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(false);

	int width, height, numChannels;

	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (numChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (numChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	this->width = width;
	this->height = height;

	stbi_image_free(data);
}

Texture::~Texture()
{
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
