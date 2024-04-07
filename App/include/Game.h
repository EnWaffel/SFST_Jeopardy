#pragma once
#include "Sprite.h"
#include <vector>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "GLBuffers.h"
#include "Defs.h"
#include "Category.h"
#include "Contestant.h"
#include <map>

#define BOARD_CATEGORIES 6


#define GAME_ERROR_GLFW_INIT 0x10

class Game
{
public:
	GLFWwindow* window;
	std::unique_ptr<GLBuffers> bufs;
	std::vector<SPRITE> sprites;
	std::unique_ptr<Shader> spriteShader;
	std::unique_ptr<Shader> textSpriteShader;
	glm::mat4 viewMatrix;
	TEXTURE tex200;
	TEXTURE tex400;
	TEXTURE tex600;
	TEXTURE tex800;
	TEXTURE tex1000;
	TEXTURE texCardBG;
	FONT fntCards;
	std::vector<std::shared_ptr<Category>> loadedCategories;
	std::vector<std::shared_ptr<Category>> categories;
	std::vector<std::unique_ptr<Contestant>> contestants;
	std::map<std::string, TEXTURE> categoryTitleTextures;
	bool gameRunning = false;
public:
	Game();
	~Game();

	int Init();
	void Terminate();

	void Run();
	void LoadTextures();
	void LoadSounds();
	void LoadFonts();
	void LoadCategories();
	void AddSprite(SPRITE& sprite);
	void RemoveSprite(SPRITE& sprite);
	void NewGame(std::vector<std::string> contestantNames);
	void SelectCategories();
	void CreateGameBoard();
	void DrawSprites();
	void DrawTextSprite(SPRITE& sprite);
};