#include "Sprite.h"
#include <iostream>

static std::string WrapText(std::string str, int location) {
	int n = str.rfind(' ', location);
	if (n != std::string::npos) {
		str.at(n) = '\n';
	}
	return str;
}

Sprite::Sprite(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<Texture> tex) : pos(pos), size(size), rotation(0.0f), scale(glm::vec2(1.0f, 1.0f)), tex(tex)
{
	if (size.x == -1 && size.y == -1)
	{
		this->size.x = tex->width;
		this->size.y = tex->height;
	}
}

void Sprite::TweenPos(const glm::vec2& to, float time, tweeny::easing::enumerated ease)
{
	posTween = tweeny::from(pos.x, pos.y).to(to.x, to.y).during(time).via(ease).onStep([this](float x, float y) {
		pos.x = x;
		pos.y = y;
		return false;
	});
	posTweenActive = true;
}


void Sprite::TweenScale(const glm::vec2& to, float time, tweeny::easing::enumerated ease)
{
	scaleTween = tweeny::from(scale.x, scale.y).to(to.x, to.y).during(time).via(ease).onStep([this](float x, float y) {
		scale.x = x;
		scale.y = y;
		return false;
	});
	scaleTweenActive = true;
}

void Sprite::TweenColor(const glm::vec4& to, float time, tweeny::easing::enumerated ease)
{
	colorTween = tweeny::from(color.r, color.g, color.b, color.a).to(to.r, to.g, to.b, to.a).during(time).via(ease).onStep([this](float r, float g, float b, float a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
		return false;
	});
	colorTweenActive = true;
}

void Sprite::PrecalculateText(glm::vec2 area)
{
	/*
	std::string wrappedText = text;
	while (true)
	{
		std::string newText = WrapText(wrappedText, 15);
		if (wrappedText == newText)
		{
			break;
		}
		wrappedText = newText;
	}

	std::vector<std::string> splitText;
	std::string buf = "";
	for (char c : wrappedText)
	{
		if (c == '\n')
		{
			splitText.push_back(buf);
			buf.clear();
			continue;
		}
		buf.push_back(c);
	}
	splitText.push_back(buf);

	int i = 0;
	for (std::string str : splitText)
	{
		float width = 0.0f;
		std::vector<float> heights;
		for (char c : str)
		{
			Glyph& glyph = font->GetGlyph(c);
			width += glyph.Size.x + glyph.Bearing.x * scale.x;
			heights.push_back(glyph.Size.y * scale.y);
		}
		std::cout << "WIDTH: " << width << std::endl;
		float avgHeight = 0.0f;
		for (float f : heights)
		{
			avgHeight += f;
		}
		avgHeight /= heights.size();
		precalculatedText.push_back({ glm::vec2(area.x / 2.0f - width / 2.0f, avgHeight * i), str });
		i++;
	}*/
}

Sprite::~Sprite()
{
}
