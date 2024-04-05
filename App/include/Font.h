#pragma once
#include <string>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Glyph
{
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

class Font
{
private:
	std::map<char, Glyph> glyphs;
public:
	Font(const std::string& path);
	~Font();

	Glyph GetGlyph(char c);
};