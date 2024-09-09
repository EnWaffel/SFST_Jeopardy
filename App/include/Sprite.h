#pragma once
#include "Defs.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <tweeny.h>
#include "Category.h"
#include "Texture.h"
#include "Font.h"

struct PrecalculatedData
{
	glm::vec2 offset;
	std::string text;
};

class Sprite
{
public:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec2 scale;
	float rotation;
	std::shared_ptr<Texture> tex;
	std::shared_ptr<Font> font = nullptr;
	std::wstring text = L"";
	glm::vec2 textArea = glm::vec2(0.0f, 0.0f);
	std::vector<PrecalculatedData> precalculatedText;
	bool visible = true;
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	std::shared_ptr<Category> cat;
	std::string pts;
	int catIdx;
	glm::vec2 __pos;
	glm::vec2 __scale;

	tweeny::tween<float, float> posTween;
	bool posTweenActive = false;
	tweeny::tween<float, float> scaleTween;
	bool scaleTweenActive = false;
	tweeny::tween<float, float, float, float> colorTween;
	bool colorTweenActive = false;
public:
	bool is_category_points = false;
	bool is_category_text = false;
public:
	Sprite(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<Texture> tex);
	~Sprite();

	void PrecalculateText(glm::vec2 area);

	void TweenPos(const glm::vec2& to, float time, tweeny::easing::enumerated ease);
	void TweenScale(const glm::vec2& to, float time, tweeny::easing::enumerated ease);
	void TweenColor(const glm::vec4& to, float time, tweeny::easing::enumerated ease);
};