#include "Sprite.h"

Sprite::Sprite(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<Texture> tex) : pos(pos), size(size), rotation(0.0f), scale(glm::vec2(1.0f, 1.0f)), tex(tex)
{
	if (size.x == -1 && size.y == -1)
	{
		this->size.x = tex->width;
		this->size.y = tex->height;
	}
}

Sprite::~Sprite()
{
}
