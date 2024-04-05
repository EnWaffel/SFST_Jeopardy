#include "Font.h"
#include "Logger.h"
#include <glad/glad.h>

Font::Font(const std::string& path)
{
    FT_Library freetype;
    if (FT_Init_FreeType(&freetype))
    {
        LogError("Failed to initialize FreeType!");
        return;
    }

    FT_Face face;
    if (FT_New_Face(freetype, path.c_str(), 0, &face))
    {
        std::string str("Failed to load font: ");
        str.append(path);
        LogWarning(str);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::string str("Failed to load glyph: ");
            str.push_back(c);
            LogWarning(str);
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Glyph glyph = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        glyphs.insert(std::pair<char, Glyph>(c, glyph));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(freetype);
}

Font::~Font()
{
    std::map<char, Glyph>::iterator it;
    for (it = glyphs.begin(); it != glyphs.end(); it++)
    {
        glDeleteTextures(1, &it->second.TextureID);
    }
}

Glyph Font::GetGlyph(char c)
{
    return glyphs.at(c);
}