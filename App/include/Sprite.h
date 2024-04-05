#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include <memory>

class Sprite
{
public:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec2 scale;
	float rotation;
	std::shared_ptr<Texture> tex;
public:
	bool is_category_points = false;
public:
	Sprite(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<Texture> tex);
	~Sprite();
};