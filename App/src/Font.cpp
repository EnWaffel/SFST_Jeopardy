#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "Font.h"
#include "Logger.h"
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <codecvt>

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

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    wchar_t alphabet[] = L"\0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!\"§$%&/()=`'*+-\\[]{}²³<>|,.-;:_ ÄäÖöÜüß";

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str = converter.to_bytes(alphabet);
    std::cout << "[INFO]: Loading Chars: " << str << std::endl;
    
    for (int i = 0; i < sizeof(alphabet) / sizeof(wchar_t); i++)
    {
        FT_UInt glyph_index = FT_Get_Char_Index(face, alphabet[i]);

        // Load glyph
        FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);

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
        glyphs.insert({ alphabet[i], glyph });
    }

    FT_Done_Face(face);
    FT_Done_FreeType(freetype);
}

Font::~Font()
{
    std::map<wchar_t, Glyph>::iterator it;
    for (it = glyphs.begin(); it != glyphs.end(); it++)
    {
        glDeleteTextures(1, &it->second.TextureID);
    }
}

Glyph& Font::GetGlyph(wchar_t c)
{
    if (glyphs.count(c) < 1)
    {
        return glyphs.at(0);
    }
    return glyphs.at(c);
}

float Font::CalculateWidth(const std::wstring& str, float scale)
{
    float width = 0.0f;

    for (wchar_t c : str)
    {
        Glyph& glyph = GetGlyph(c);
        width += (glyph.Advance >> 6) * scale;
    }

    return width;
}
