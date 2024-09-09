#pragma once
#include <memory>
#include "Texture.h"
#include "Sprite.h"
#include "Logger.h"
#include "Sound.h"
#include "Font.h"

#define TEXTURE std::shared_ptr<Texture>
#define SPRITE std::shared_ptr<Sprite>
#define SOUND std::unique_ptr<Sound>
#define FONT std::shared_ptr<Font>

#define MAKE_TEX(path) std::make_shared<Texture>(path)
#define MAKE_TEXlog(path) std::make_shared<Texture>(path); LogInfo("Texture: " + std::string(path))
#define MAKE_SPRITE(pos, size, tex) std::make_shared<Sprite>(pos, size, tex)
#define MAKE_SOUND(path) std::make_unique<Sound>(path)
#define MAKE_SOUNDlog(path) std::make_unique<Sound>(path); LogInfo("Sound: " + std::string(path))
#define MAKE_FONT(path) std::make_shared<Font>(path)
#define MAKE_FONTlog(path) std::make_shared<Font>(path); LogInfo("Font: " + std::string(path))