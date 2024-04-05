#include <glad/glad.h>
#include "Game.h"
#include "Logger.h"
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <random>
#include <iostream>
#include <math.h>
namespace fs = std::filesystem;

#define WIDTH 1280.0f
#define HEIGHT 800.0f

Game::Game() : viewMatrix(glm::mat4(1.0f)), window(nullptr)
{
}

Game::~Game()
{
}

static void OnResizeCallback(GLFWwindow* window, int width, int height)
{
	float ratioX = width / (float)WIDTH;
	float ratioY = height / (float)HEIGHT;
	float ratio = fminf(ratioX, ratioY);

	int viewWidth = (int)(WIDTH * ratio);
	int viewHeight = (int)(HEIGHT * ratio);

	int viewX = (int)(width - WIDTH * ratio) / 2;
	int viewY = (int)(height - HEIGHT * ratio) / 2;

	glViewport(viewX, viewY, viewWidth, viewHeight);
}

int Game::Init()
{
	LogInfo("[Game]: Initializing...");

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Game Window", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return GAME_ERROR_GLFW_INIT;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, vidMode->width / 2 - WIDTH / 2, vidMode->height / 2 - HEIGHT / 2);
	glfwSwapInterval(0);

	LogInfo("[Game]: Setting up...");
	viewMatrix = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f);

	LogInfo("[Game]: Creating buffers...");
	bufs = std::make_unique<GLBuffers>();
	
	spriteShader = std::make_unique<Shader>("assets/shaders/vertex_sprite.glsl", "assets/shaders/fragment_sprite.glsl");

	LoadTextures();
	LoadCategories();
	
	glfwShowWindow(window);
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);

	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

	Run();
	return 0;
}

void Game::Terminate()
{
	LogInfo("[Game]: Disabling...");

	sprites.clear();

	LogInfo("[Game]: Disabled!");
}

void Game::Run()
{
	LogInfo("[Game]: Running!");

	std::vector<std::string> players = { "Test1", "Test2", "Test3"};
	NewGame(players);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (SPRITE sprite : sprites)
		{
			spriteShader->Use();
			spriteShader->SetMatrix("cam", glm::mat4(1.0f));
			spriteShader->SetMatrix("view", viewMatrix);

			glm::mat4 model = glm::mat4(1.0f);

			glm::vec2 diff = glm::vec2((sprite->size.x * sprite->scale.x) - sprite->size.x, (sprite->size.y * sprite->scale.y) - sprite->size.y);

			model = glm::translate(model, glm::vec3(sprite->pos.x - (diff.x / 2.0f), sprite->pos.y - (diff.y / 2.0f), 0.0f));

			model = glm::translate(model, glm::vec3(sprite->size.x / 2.0f * sprite->scale.x, sprite->size.y / 2.0f * sprite->scale.y, 0.0f));
			model = glm::rotate(model, glm::radians(sprite->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-(sprite->size.x / 2.0f) * sprite->scale.x, -(sprite->size.y / 2.0f) * sprite->scale.y, 0.0f));

			model = glm::scale(model, glm::vec3(sprite->size.x + diff.x, sprite->size.y + diff.y, 0.0f));

			spriteShader->SetMatrix("model", model);

			if (sprite->tex != nullptr)
			{
				glActiveTexture(GL_TEXTURE0);
				sprite->tex->Bind();
			}

			glBindVertexArray(bufs->VAO_rect);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			if (sprite->tex != nullptr)
			{
				sprite->tex->Unbind();
			}
		}
		glfwSwapBuffers(window);
	}
}

void Game::LoadTextures()
{
	LogInfo("[Game]: Loading textures...");

	tex200 = MAKE_TEXlog("assets/images/200.png");
	tex400 = MAKE_TEXlog("assets/images/400.png");
	tex600 = MAKE_TEXlog("assets/images/600.png");
	tex800 = MAKE_TEXlog("assets/images/800.png");
	tex1000 = MAKE_TEXlog("assets/images/1000.png");
	texCardBG = MAKE_TEXlog("assets/images/card_bg.png");
}

void Game::LoadSounds()
{
	LogInfo("[Game]: Loading sounds...");
}

void Game::LoadFonts()
{
	LogInfo("[Game]: Loading fonts...");
}

void Game::LoadCategories()
{
	LogInfo("[Game]: Loading categories...");
	
	for (const fs::directory_entry& entry : fs::directory_iterator("assets/categories"))
	{
		loadedCategories.push_back(Category::LoadFromFile(entry.path().string()));
	}
}

void Game::AddSprite(SPRITE& sprite)
{
	sprites.push_back(sprite);
}

void Game::RemoveSprite(SPRITE& sprite)
{
	sprites.erase(std::find(sprites.begin(), sprites.end(), sprite));
}

void Game::NewGame(std::vector<std::string> contestantNames)
{
	LogInfo("[Game]: Setting up new game...");
	categories.clear();
	contestants.clear();

	// Make / Add Contestants
	for (std::string& s : contestantNames)
	{
		std::unique_ptr<Contestant> c = std::make_unique<Contestant>(s);
		contestants.push_back(std::move(c));
	}

	LogInfo("[Game]: Selecting categories...");
	SelectCategories();

	LogInfo("[Game]: Creating game board...");
	CreateGameBoard();
}

void Game::SelectCategories()
{
	std::srand(std::time(nullptr));
	for (int i = 0; i < BOARD_CATEGORIES; i++)
	{
		std::shared_ptr<Category> c = nullptr;
		//do
		//{
			c = loadedCategories[std::rand() % loadedCategories.size()];
		//} while (std::find(categories.begin(), categories.end(), c) == categories.end());
		categories.push_back(c);
	}
}

void Game::CreateGameBoard()
{
	for (int i = 0; i < BOARD_CATEGORIES; i++)
	{
		float x = texCardBG->width * i;
		SPRITE cSprite = MAKE_SPRITE(glm::vec2(x, 8), glm::vec2(-1.0f, -1.0f), texCardBG);
		cSprite->rotation = std::rand() % 2 == 0 ? 0.0f : 180.0f;
		cSprite->scale = glm::vec2(0.95f, 0.95f);
		AddSprite(cSprite);

		SPRITE sprite200 = MAKE_SPRITE(glm::vec2(x, 24 + tex200->height), glm::vec2(-1.0f, -1.0f), tex200);
		SPRITE sprite400 = MAKE_SPRITE(glm::vec2(x, 24 + tex400->height * 2), glm::vec2(-1.0f, -1.0f), tex400);
		SPRITE sprite600 = MAKE_SPRITE(glm::vec2(x, 24 + tex600->height * 3), glm::vec2(-1.0f, -1.0f), tex600);
		SPRITE sprite800 = MAKE_SPRITE(glm::vec2(x, 24 + tex800->height * 4), glm::vec2(-1.0f, -1.0f), tex800);
		SPRITE sprite1000 = MAKE_SPRITE(glm::vec2(x, 24 + tex1000->height * 5), glm::vec2(-1.0f, -1.0f), tex1000);
		sprite200->scale = glm::vec2(0.95f, 0.95f);
		sprite400->scale = glm::vec2(0.95f, 0.95f);
		sprite600->scale = glm::vec2(0.95f, 0.95f);
		sprite800->scale = glm::vec2(0.95f, 0.95f);
		sprite1000->scale = glm::vec2(0.95f, 0.95f);

		sprite200->is_category_points = true;
		sprite400->is_category_points = true;
		sprite600->is_category_points = true;
		sprite800->is_category_points = true;
		sprite1000->is_category_points = true;

		AddSprite(sprite200);
		AddSprite(sprite400);
		AddSprite(sprite600);
		AddSprite(sprite800);
		AddSprite(sprite1000);

		LogInfo("[Game]: Created column: " + std::to_string(i + 1));
	}
}
