#pragma once
#include "Defs.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

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
	TEXTURE tex;
	FONT font = nullptr;
	std::string text = "";
	glm::vec2 textArea = glm::vec2(0.0f, 0.0f);
	std::vector<PrecalculatedData> precalculatedText;
public:
	bool is_category_points = false;
public:
	Sprite(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<Texture> tex);
	~Sprite();

	void PrecalculateText(glm::vec2 area);
};