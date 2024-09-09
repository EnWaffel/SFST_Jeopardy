#pragma once
#include <string>

class Texture
{
private:
	unsigned int id;
public:
	int width;
	int height;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind();
	void Unbind();
};