#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <glad/glad.h>
#include "Game.h"
#include "Logger.h"
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <random>
#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>
#include <codecvt>
#include "WebInterface.h"
namespace fs = std::filesystem;

#define WWM_SOUND(snd) if (wwmSounds) { PlayWWMSound(snd); }
#define S_WWM_SOUND(snd) if (wwmSounds) { StopWWMSound(snd); }

bool __REQ_SYNC = false;
bool __SYNC = false;
bool __PAUSE = false;

#define WIDTH 1280.0f
#define HEIGHT 800.0f

static float FPS = 60.0f;
static bool fullscreen = false;

std::string ConvertWStringToString(const std::wstring& str)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(str);
}

static void SyncAndPause()
{
	__REQ_SYNC = true;
	while (!__SYNC);
	__SYNC = false;
	__PAUSE = true;
}

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

static int mini(int x, int y)
{
	return x < y ? x : y;
}

static int maxi(int x, int y)
{
	return x > y ? x : y;
}

static GLFWmonitor* GetCurrentMonitor(GLFWwindow* window)
{
	int nmonitors, i;
	int wx, wy, ww, wh;
	int mx, my, mw, mh;
	int overlap, bestoverlap;
	GLFWmonitor* bestmonitor;
	GLFWmonitor** monitors;
	const GLFWvidmode* mode;

	bestoverlap = 0;
	bestmonitor = NULL;

	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	monitors = glfwGetMonitors(&nmonitors);

	for (i = 0; i < nmonitors; i++) {
		mode = glfwGetVideoMode(monitors[i]);
		glfwGetMonitorPos(monitors[i], &mx, &my);
		mw = mode->width;
		mh = mode->height;

		overlap =
			maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) *
			maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

		if (bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
}

static void OnKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_F11)
		{
			GLFWmonitor* monitor = GetCurrentMonitor(window);
			if (!fullscreen)
			{
				fullscreen = true;

				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			else
			{
				fullscreen = false;

				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, NULL, (mode->width - WIDTH) / 2, (mode->height - HEIGHT) / 2, WIDTH, HEIGHT, 0);
			}
		}
	}
}



int Game::Init()
{
	LogInfo("[Game]: Initializing...");

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

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

	FPS = vidMode->refreshRate;

	LogInfo("[Game]: Setting up...");
	viewMatrix = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f);

	LogInfo("[Game]: Creating buffers...");
	bufs = std::make_unique<GLBuffers>();
	
	spriteShader = std::make_unique<Shader>("assets/shaders/vertex_sprite.glsl", "assets/shaders/fragment_sprite.glsl");
	textSpriteShader = std::make_unique<Shader>("assets/shaders/vertex_text.glsl", "assets/shaders/fragment_text.glsl");

	LoadTextures();
	LoadSounds();
	LoadFonts();
	LoadCategories();
	
	glfwShowWindow(window);
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);
	glfwSetKeyCallback(window, OnKeyCallback);

	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	Run();

	try
	{
	}
	catch (std::exception e)
	{
		LogError(std::string("Game::Init(): Caught exception in Game::Run(): ") + e.what());
		
	}

	return 0;
}

void Game::Terminate()
{
	LogInfo("[Game]: Disabling...");

	WebInterface::End();
	tm.Save();
	sprites.clear();

	LogInfo("[Game]: Disabled!");
}

void Game::Run()
{
	LogInfo("[Game]: Running!");

	emptyBG = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(WIDTH, HEIGHT), texBGEmpty);
	AddSprite(emptyBG);

	joker = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f), texJoker);

	vignette1 = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(WIDTH, HEIGHT), texVignetteCorrect);
	vignette1->visible = false;

	countdownText = MAKE_SPRITE(glm::vec2(100.0f, 100.0f), glm::vec2(0.0f, 0.0f), nullptr);
	countdownText->font = fntCards;
	countdownText->text = L"0";
	countdownText->scale = glm::vec2(1.5f, 1.5f);

	bool jokerOut = true;
	float countdownFlashTime = 0.35f;
	bool countdownFlash = false;
	int lastCountdown = 0;

	double last = glfwGetTime() * 1000.0;
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime() * 1000.0;
		if (now >= last + 1000.0 / FPS)
		{
			if (__REQ_SYNC)
			{
				__SYNC = true;
				__REQ_SYNC = false;
			}
			if (__PAUSE)
			{
				continue;
			}

			float delta = now - last;
			last = now;

			glfwPollEvents();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (SPRITE& s : spritesToRemove)
			{
				SPRITE* __s = nullptr;
				for (SPRITE& _s : sprites)
				{
					if (s.get() == _s.get())
					{
						__s = &_s;
						break;
					}
				}
				if (__s == nullptr) continue;
				RemoveSprite(*__s);
			}
			spritesToRemove.clear();

			if (jokerAnim)
			{
				if (!joker->scaleTweenActive)
				{
					if (jokerOut)
					{
						jokerOut = false;
						joker->TweenScale(glm::vec2(1.5f, 1.5f), 500.0f, tweeny::easing::enumerated::sinusoidalInOut);
					}
					else
					{
						jokerOut = true;
						joker->TweenScale(glm::vec2(1.35f, 1.35f), 500.0f, tweeny::easing::enumerated::sinusoidalInOut);
					}
				}
			}

			if (countdownActive)
			{
				countdown -= 0.001f * delta;
				
				if (countdown <= 16)
				{
					countdownFlashTime -= 0.001f * delta;
					if (countdownFlashTime <= 0.0f)
					{
						countdownFlashTime = 0.35f;
						if (countdownFlash)
						{
							countdownText->TweenColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1000.0f * 0.35f, tweeny::easing::enumerated::quarticOut);
						}
						else
						{
							countdownText->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f * 0.35f, tweeny::easing::enumerated::quarticOut);
						}
						countdownFlash = !countdownFlash;
					}
				}

				if (countdown <= 0)
				{
					state = COUNTDOWN_OUT;
					countdownActive = false;
				}
				else
				{
					if (lastCountdown != (int)countdown)
					{
						lastCountdown = (int)countdown;
						countdownText->text = std::to_wstring((int)countdown);
						float w = countdownText->font->CalculateWidth(countdownText->text, 1.5f);
						countdownText->pos = glm::vec2(WIDTH / 2.0f - w / 2.0f, 785.0f);
					}
				}
			}

			for (SPRITE& sprite : sprites)
			{
				if (sprite->posTweenActive)
				{
					if (sprite->posTween.progress() >= 1.0f)
					{
						sprite->posTweenActive = false;
					}
					sprite->posTween.step((int)delta);
				}
				if (sprite->scaleTweenActive)
				{
					if (sprite->scaleTween.progress() >= 1.0f)
					{
						sprite->scaleTweenActive = false;
					}
					sprite->scaleTween.step((int)delta);
				}
				if (sprite->colorTweenActive)
				{
					if (sprite->colorTween.progress() >= 1.0f)
					{
						sprite->colorTweenActive = false;
					}
					sprite->colorTween.step((int)delta);
				}
			}

			DrawSprites();
			glfwSwapBuffers(window);
		}
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
	texBGEmpty = MAKE_TEXlog("assets/images/bg_empty.jpg");
	texOverlayBlack = MAKE_TEXlog("assets/images/overlay_black.png");
	texJoker = MAKE_TEXlog("assets/images/joker.png");
	texVignetteCorrect = MAKE_TEXlog("assets/images/correctVignette.png");
	texVignetteWrong = MAKE_TEXlog("assets/images/wrongVignette.png");
	texOverlayWhite = MAKE_TEXlog("assets/images/overlay_white.png");

	categoryTitleTextures.insert({ "", texCardBG });
}

void Game::LoadSounds()
{
	LogInfo("[Game]: Loading sounds...");

	sndBoardFill = MAKE_SOUNDlog("assets/sounds/board_fill.ogg");
	sndBuzzer = MAKE_SOUNDlog("assets/sounds/buzzer.ogg");

	wwmSoundsMap.insert({ "guessing", MAKE_SOUND("assets/sounds/wwm/guessing.ogg") });
	wwmSoundsMap.insert({ "confirm", MAKE_SOUND("assets/sounds/wwm/confirm.ogg") });
	wwmSoundsMap.insert({ "correct", MAKE_SOUND("assets/sounds/wwm/correct.ogg") });
	wwmSoundsMap.insert({ "wrong", MAKE_SOUND("assets/sounds/wwm/wrong.ogg") });
	wwmSoundsMap.insert({ "selecting", MAKE_SOUND("assets/sounds/wwm/selecting.ogg") });
}

void Game::LoadFonts()
{
	LogInfo("[Game]: Loading fonts...");

	fntCards = MAKE_FONTlog("assets/fonts/ConsolaMono-Bold.ttf");
}

void Game::LoadCategories()
{
	LogInfo("[Game]: Loading categories...");
	
	for (const fs::directory_entry& entry : fs::directory_iterator("assets/categories"))
	{
		if (entry.is_directory()) continue;
		std::string str = entry.path().string();
		std::shared_ptr<Category> c = Category::LoadFromFile(str);
		categoryTitleTextures.insert({ c->title, MAKE_TEX("assets/images/category_title__" + c->title + ".png")});
		loadedCategories.push_back(c);
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
	curSelectingContestant = -1;
	tm.contestants = contestantNames.size();

	StartBBMXScript("scripts/reset.lua");
	StartBBMXScript("scripts/sfst/jeopardy/white.lua");

	for (std::string& s : contestantNames)
	{
		std::unique_ptr<Contestant> c = std::make_unique<Contestant>(s);
		contestants.push_back(std::move(c));
	}

	overlay1 = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(WIDTH, HEIGHT), texOverlayBlack);

	LogInfo("[Game]: Selecting categories...");
	SelectCategories();

	LogInfo("[Game]: Creating game board...");
	CreateGameBoard();

	for (SPRITE& sprite : sprites)
	{
		if (sprite->is_category_text) cateSprites.push_back(sprite);
	}
	PlayBoardFillAnimation();

	state = REVEAL_CATEGORIES;

	gameRunning = true;

	std::vector<int> points;
	for (int i = 0; i < contestants.size(); i++) points.push_back(0);
	WebInterface::UpdatePoints(points);
	WebInterface::SetContestants(contestantNames);
	WebInterface::Start();
}

void Game::PlayBoardFillAnimation()
{
	std::vector<SPRITE> pointSprites;
	for (SPRITE& sprite : sprites)
	{
		if (sprite->is_category_points) pointSprites.push_back(sprite);
	}

	sndBoardFill->Play();
	std::this_thread::sleep_for(std::chrono::milliseconds(122));
	std::srand(std::time(nullptr));
	for (int i = 0; i < 5; i++)
	{
		int n = 4;
		if (i == 0) n = 5;

		for (int j = 0; j < n; j++)
		{
			int rnd = std::rand() % pointSprites.size();
			pointSprites[rnd]->visible = true;
			pointSprites[rnd]->scale = glm::vec2(0.0f, 0.0f);
			pointSprites[rnd]->TweenScale(glm::vec2(0.954f, 0.954f), 400.0f, tweeny::easing::enumerated::exponentialOut);
			pointSprites.erase(pointSprites.begin() + rnd);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}

	for (SPRITE& sprite : pointSprites)
	{
		sprite->visible = true;
		sprite->scale = glm::vec2(0.0f, 0.0f);
		sprite->TweenScale(glm::vec2(0.954f, 0.954f), 400.0f, tweeny::easing::enumerated::exponentialOut);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

void Game::SelectCategories()
{
	std::srand(std::time(nullptr));

	bool lessCategories = false;
	if (loadedCategories.size() < BOARD_CATEGORIES)
	{
		lessCategories = true;
		LogWarning("loadedCategories.size() (" + std::to_string(loadedCategories.size()) + ") < BOARD_CATEGORIES (5); filling with empty ones");
	}

	int catCount = lessCategories ? loadedCategories.size() : BOARD_CATEGORIES;
	for (int i = 0; i < catCount; i++)
	{
		std::shared_ptr<Category> c = nullptr;
		do
		{
			c = loadedCategories[std::rand() % loadedCategories.size()];
		} while (std::find(categories.begin(), categories.end(), c) != categories.end());
		categories.push_back(c);
	}

	if (lessCategories)
	{
		for (int i = 0; i < BOARD_CATEGORIES - loadedCategories.size(); i++)
		{
			std::shared_ptr<Category> c = std::make_shared<Category>();
			categories.push_back(c);
		}
	}
}

void Game::CreateGameBoard()
{
	for (int i = 0; i < BOARD_CATEGORIES; i++)
	{
		float x = texCardBG->width * i;
		std::string s(categories[i]->title.begin(), categories[i]->title.end());
		SPRITE text = MAKE_SPRITE(glm::vec2(x, 8), glm::vec2(-1.0f, -1.0f), categoryTitleTextures[s]);
		text->scale = glm::vec2(0.954f, 0.954f);
		text->visible = false;
		text->is_category_text = true;
		text->cat = categories[i];
		text->catIdx = i;
		AddSprite(text);

		bool isEmptyCategory = categories[i]->title.empty();

		SPRITE sprite200 = MAKE_SPRITE(glm::vec2(x, 24 + tex200->height), glm::vec2(-1.0f, -1.0f), isEmptyCategory ? texCardBG : tex200);
		SPRITE sprite400 = MAKE_SPRITE(glm::vec2(x, 24 + tex400->height * 2), glm::vec2(-1.0f, -1.0f), isEmptyCategory ? texCardBG : tex400);
		SPRITE sprite600 = MAKE_SPRITE(glm::vec2(x, 24 + tex600->height * 3), glm::vec2(-1.0f, -1.0f), isEmptyCategory ? texCardBG : tex600);
		SPRITE sprite800 = MAKE_SPRITE(glm::vec2(x, 24 + tex800->height * 4), glm::vec2(-1.0f, -1.0f), isEmptyCategory ? texCardBG : tex800);
		SPRITE sprite1000 = MAKE_SPRITE(glm::vec2(x, 24 + tex1000->height * 5), glm::vec2(-1.0f, -1.0f), isEmptyCategory ? texCardBG : tex1000);

		sprite200->pts = "200";
		sprite400->pts = "400";
		sprite600->pts = "600";
		sprite800->pts = "800";
		sprite1000->pts = "1000";

		sprite200->cat = categories[i];
		sprite400->cat = categories[i];
		sprite600->cat = categories[i];
		sprite800->cat = categories[i];
		sprite1000->cat = categories[i];
		sprite200->catIdx = i;
		sprite400->catIdx = i;
		sprite600->catIdx = i;
		sprite800->catIdx = i;
		sprite1000->catIdx = i;

		sprite200->scale = glm::vec2(0.954f, 0.954f);
		sprite400->scale = glm::vec2(0.954f, 0.954f);
		sprite600->scale = glm::vec2(0.954f, 0.954f);
		sprite800->scale = glm::vec2(0.954f, 0.954f);
		sprite1000->scale = glm::vec2(0.954f, 0.954f);

		sprite200->is_category_points = true;
		sprite400->is_category_points = true;
		sprite600->is_category_points = true;
		sprite800->is_category_points = true;
		sprite1000->is_category_points = true;
		
		sprite200->visible = false;
		sprite400->visible = false;
		sprite600->visible = false;
		sprite800->visible = false;
		sprite1000->visible = false;

		if (std::find(categories[i]->excludedQuestions.begin(), categories[i]->excludedQuestions.end(), "200") == categories[i]->excludedQuestions.end()) AddSprite(sprite200);
		if (std::find(categories[i]->excludedQuestions.begin(), categories[i]->excludedQuestions.end(), "400") == categories[i]->excludedQuestions.end()) AddSprite(sprite400);
		if (std::find(categories[i]->excludedQuestions.begin(), categories[i]->excludedQuestions.end(), "600") == categories[i]->excludedQuestions.end()) AddSprite(sprite600);
		if (std::find(categories[i]->excludedQuestions.begin(), categories[i]->excludedQuestions.end(), "800") == categories[i]->excludedQuestions.end()) AddSprite(sprite800);
		if (std::find(categories[i]->excludedQuestions.begin(), categories[i]->excludedQuestions.end(), "1000") == categories[i]->excludedQuestions.end()) AddSprite(sprite1000);

		LogInfo("[Game]: Created column: " + std::to_string(i + 1));
	}
}

void Game::DrawSprites()
{
	for (int i = 0; i < sprites.size(); i++)
	{
		SPRITE& sprite = sprites[i];

		if (sprite == nullptr) continue;
		if (sprite.get() == nullptr) continue;

		if (!sprite->visible) continue;
		if (sprite->font != nullptr)
		{
			DrawTextSprite(sprite);
			continue;
		}

		spriteShader->Use();
		spriteShader->SetMatrix("cam", glm::mat4(1.0f));
		spriteShader->SetMatrix("view", viewMatrix);
		spriteShader->SetVec4("color", sprite->color);

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
}

void Game::DrawTextSprite(SPRITE& sprite)
{
	textSpriteShader->Use();
	textSpriteShader->SetMatrix("cam", glm::mat4(1.0f));
	textSpriteShader->SetMatrix("view", viewMatrix);
	textSpriteShader->SetVec4("col", sprite->color);

	float xOffset = 0.0f;
	float yOffset = 0.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (wchar_t c : sprite->text)
	{
		Glyph& glyph = sprite->font->GetGlyph(c);

		float xpos = sprite->pos.x + xOffset + glyph.Bearing.x * sprite->scale.x;
		float ypos = sprite->pos.y + yOffset - glyph.Bearing.y * sprite->scale.y;

		float w = glyph.Size.x * sprite->scale.x;
		float h = glyph.Size.y * sprite->scale.y;

		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(xpos, ypos, 0));
		model = glm::scale(model, glm::vec3(w, h, 0));

		textSpriteShader->SetMatrix("model", model);

		glBindTexture(GL_TEXTURE_2D, glyph.TextureID);

		glBindVertexArray(bufs->VAO_rect);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		xOffset += (glyph.Advance >> 6) * sprite->scale.x;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::RevealNextCategory()
{
	if (state != REVEAL_CATEGORIES)
	{
		LogWarning("Tried to reaveal a category while not in state: REVEAL_CATEGORIES");
		return;
	}

	SPRITE& sprite = cateSprites[catesRevealed];

	AddSprite(overlay1);
	overlay1->visible = true;

	RemoveSprite(sprite);
	AddSprite(sprite);

	glm::vec2 originalPos = sprite->pos;
	glm::vec2 originalScale = sprite->scale;

	sprite->visible = true;
	sprite->pos = glm::vec2(WIDTH / 2.0f - sprite->tex->width / 2.0f, HEIGHT / 2.0f - sprite->tex->height / 2.0f);
	sprite->scale = glm::vec2(2.0f, 2.0f);

	if (quickAnims)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	overlay1->visible = false;
	RemoveSprite(overlay1);
	if (quickAnims)
	{
		sprite->TweenPos(originalPos, 100.0f, tweeny::easing::enumerated::exponentialInOut);
		sprite->TweenScale(originalScale, 100.0f, tweeny::easing::enumerated::exponentialInOut);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	else
	{
		sprite->TweenPos(originalPos, 1000.0f, tweeny::easing::enumerated::exponentialInOut);
		sprite->TweenScale(originalScale, 1000.0f, tweeny::easing::enumerated::exponentialInOut);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	catesRevealed++;

	if (catesRevealed >= cateSprites.size())
	{
		state = PLAYER_NEXT;
		Next(PLAYER_NEXT, nullptr);
	}
}

int __call_failed = 0;

void Game::Next(State targetState, void* data)
{
	if (state != targetState)
	{
		__call_failed++;
		LogWarning("Game::Next(): tried to call: " + StateToString(targetState) + " but current state is: " + StateToString(state));
		if (__call_failed >= 100)
		{
			MessageBox(nullptr, L"Game::Next() call failed too many times!", L"Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		return;
	}

	__call_failed = 0;
	LogInfo("Game::Next(): state: " + StateToString(state));

	if (state == REVEAL_CATEGORIES)
	{
		RevealNextCategory();
	}
	else if (state == PLAYER_NEXT)
	{
		StartBBMXScript("scripts/sfst/jeopardy/white.lua");
		//WWM_SOUND("selecting");

		std::cout << "----- POINTS -----" << std::endl;
		for (std::unique_ptr<Contestant>& c : contestants)
		{
			std::cout << "- " << c->name << ": " << c->points << std::endl;
		}

		std::vector<int> points;
		for (const std::unique_ptr<Contestant>& contestant : contestants)
		{
			points.push_back(contestant->points);
		}
		WebInterface::UpdatePoints(points);

		if (curSelectingContestant >= contestants.size())
		{
			curSelectingContestant = 0;
		}

		std::cout << "Next Contestant: " << contestants[curSelectingContestant]->name << std::endl;

		for (int i = 0; i < contestants.size(); i++)
		{
			if (i == curSelectingContestant) continue;
			uint8_t buf[] = { 1, SERIAL_ACTION_COL_UPDATE, i, 0 };
			ser->Write(buf, sizeof(buf));
		}

		uint8_t buf[] = { 1, SERIAL_ACTION_COL_UPDATE, curSelectingContestant, 3 };
		ser->Write(buf, sizeof(buf));

		state = PLAYER_CAT_SELECT;
	}
	else if (state == PLAYER_CAT_SELECT)
	{
		state = AWAIT_BUZZ;
		SelectCatAndPoints();
	}
	else if (state == AWAIT_BUZZ)
	{
		uint8_t buf[] = { 1, SERIAL_ACTION_BUZZER_ACTIVE, 0 };
		ser->Write(buf, sizeof(buf));

		SetLogColor(COLOR_FOREGROUND_GREEN);
		LogInfo("BUZZED!");
		sndBuzzer->Play();
		countdownActive = false;
		countdownText->color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		countdownText->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 750.0f, tweeny::easing::enumerated::quarticOut);
		state = AWAIT_BUZZ_CONFIRM;
	}
	else if (state == AWAIT_BUZZ_CONFIRM)
	{
		guessingContestant = *(int*)data;
		state = AWAIT_CORRECT_OR_WRONG;
		S_WWM_SOUND("guessing");
		WWM_SOUND("confirm");
		StartBBMXScript("scripts/sfst/jeopardy/yellow.lua");
	}
	else if (state == AWAIT_CORRECT_OR_WRONG)
	{
		S_WWM_SOUND("confirm");
		bool correct = *(bool*)data;
		if (correct)
		{
			tm.correctGuesses++;
			WWM_SOUND("correct");
			vignette1->tex = texVignetteCorrect;
			contestants[guessingContestant]->points += winPts;
			curSelectingContestant = guessingContestant;
			WebInterface::DisableQuestion();
			StartBBMXScript("scripts/sfst/jeopardy/green.lua");
		}
		else
		{
			tm.wrongGuesses++;
			WWM_SOUND("wrong");
			vignette1->tex = texVignetteWrong;
			contestants[guessingContestant]->points -= winPts;
			uint8_t buf[] = { 1, SERIAL_ACTION_BUZZER_ACTIVE, 1 };
			ser->Write(buf, sizeof(buf));
			StartBBMXScript("scripts/sfst/jeopardy/red.lua");
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			StartBBMXScript("scripts/sfst/jeopardy/white.lua");
		}
		
		if (!joker)
		{
			vignette1->visible = true;
			vignette1->color.a = 1.0f;
			vignette1->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);
			if (wwmSounds)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(3850));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(850));
			}
			vignette1->visible = false;
		}

		if (!correct)
		{
			countdownActive = true;
			state = AWAIT_BUZZ;
			WWM_SOUND("guessing");
		}
		else
		{
			DeselectCatAndPoints();
			state = PLAYER_NEXT;
			Next(PLAYER_NEXT, nullptr);
		}
	}

	else if (state == COUNTDOWN_OUT)
	{
		countdownActive = false;
		S_WWM_SOUND("guessing");
		WebInterface::DisableQuestion();
		DeselectCatAndPoints();
		curSelectingContestant++;
		state = PLAYER_NEXT;
		Next(PLAYER_NEXT, nullptr);
	}
	else
	{
		LogWarning("Game::Next(): tried to step not (yet) implemented state! (" + StateToString(state) + ")");
	}
}

std::string Game::StateToString(const State& state)
{
	switch (state)
	{
	case REVEAL_CATEGORIES: return "REVEAL_CATEGORIES";
	case PLAYER_NEXT: return "PLAYER_NEXT";
	case PLAYER_CAT_SELECT: return "PLAYER_CAT_SELECT";
	case AWAIT_BUZZ: return "AWAIT_BUZZ";
	case AWAIT_CORRECT_OR_WRONG: return "AWAIT_CORRECT_OR_WRONG";
	case AWAIT_BUZZ_CONFIRM: return "AWAIT_BUZZ_CONFIRM";
	case COUNTDOWN_OUT: return "COUNTDOWN_OUT";
	default: return "Unknown State";
	}
}

void Game::SelectCatAndPoints()
{
	std::string selectedPts;

	switch (selectedPoints)
	{
	case 0:
	{
		selectedPts = "200";
		break;
	}
	case 1:
	{
		selectedPts = "400";
		break;
	}
	case 2:
	{
		selectedPts = "600";
		break;
	}
	case 3:
	{
		selectedPts = "800";
		break;
	}
	case 4:
	{
		selectedPts = "1000";
		break;
	}
	}

	for (SPRITE& s : sprites)
	{
		if (!s->is_category_text) continue;
		if (s->catIdx != selectedCategoryIdx) continue;

		titleSprite = s;
		break;
	}

	for (SPRITE& s : sprites)
	{
		if (!s->is_category_points) continue;
		if (s->catIdx != selectedCategoryIdx) continue;
		if (s->pts != selectedPts) continue;

		ptsSprite = s;
		break;
	}

	switch (selectedPoints)
	{
	case 0:
	{
		curQuestion = titleSprite->cat->q200;
		winPts = 200;
		break;
	}
	case 1:
	{
		curQuestion = titleSprite->cat->q400;
		winPts = 400;
		break;
	}
	case 2:
	{
		curQuestion = titleSprite->cat->q600;
		winPts = 600;
		break;
	}
	case 3:
	{
		curQuestion = titleSprite->cat->q800;
		winPts = 800;
		break;
	}
	case 4:
	{
		curQuestion = titleSprite->cat->q1000;
		winPts = 1000;
		break;
	}
	}

	bool isJoker = curQuestion.isJoker;

	textSprites.clear();

	if (!isJoker)
	{
		textSprites = GenerateQuestionText(curQuestion.q, titleSprite->cat);
	}
	else
	{
		jokerAnim = true;
		joker->pos = glm::vec2(WIDTH / 2.0f - texJoker->width / 2.0f, 500.0f);
	}

	RemoveSprite(titleSprite);
	RemoveSprite(ptsSprite);

	AddSprite(overlay1);
	overlay1->color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	overlay1->visible = true;
	overlay1->TweenColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);

	AddSprite(vignette1);

	AddSprite(titleSprite);
	AddSprite(ptsSprite);

	countdownText->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!isJoker)
	{
		for (SPRITE& sprite : textSprites)
		{
			sprite->color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			sprite->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1500.0f, tweeny::easing::enumerated::linear);
			AddSprite(sprite);
		}
	}
	else
	{
		AddSprite(joker);
	}

	titleSprite->__pos = titleSprite->pos;
	ptsSprite->__pos = ptsSprite->__pos;

	titleSprite->TweenPos(glm::vec2(WIDTH / 2.0f - titleSprite->tex->width / 2.0f, 100.0f), 1500.0f, tweeny::easing::enumerated::exponentialInOut);
	ptsSprite->TweenPos(glm::vec2(WIDTH / 2.0f - ptsSprite->tex->width / 2.0f, 255.0f), 1500.0f, tweeny::easing::enumerated::exponentialInOut);
	titleSprite->TweenScale(glm::vec2(1.5f, 1.5f), 1500.0f, tweeny::easing::enumerated::exponentialInOut);

	if (!isJoker)
	{
		uint8_t buf[] = { 1, SERIAL_ACTION_BUZZER_ACTIVE, 1 };
		ser->Write(buf, sizeof(buf));
	}
	else
	{
		uint8_t buf[] = { 1, SERIAL_ACTION_BUZZER_ACTIVE, 0 };
		ser->Write(buf, sizeof(buf));
	}

	WebInterface::EnableQuestion(curQuestion.q, curQuestion.correctAnswer);

	if (!isJoker)
	{
		countdown = 46.0f;
		countdownActive = true;
		AddSprite(countdownText);
		WWM_SOUND("guessing");
		//S_WWM_SOUND("selecting");
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		state = AWAIT_CORRECT_OR_WRONG;
		guessingContestant = curSelectingContestant;
		bool correct = true;
		Next(AWAIT_CORRECT_OR_WRONG, &correct);
	}
}

void Game::DeselectCatAndPoints()
{
	__REQ_SYNC = true;
	while (!__SYNC)
	{
	}
	__SYNC = false;

	if (qImageShown)
	{
		HideQuestionImage();
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

	bool isJoker = curQuestion.isJoker;

	if (isJoker)
	{
		spritesToRemove.push_back(joker);
	}
	spritesToRemove.push_back(countdownText);

	for (SPRITE& s : textSprites)
	{
		s->visible = false;
		spritesToRemove.push_back(s);
	}
	textSprites.clear();

	if (answerShown)
	{
		HideAnswer();
	}

	titleSprite->TweenPos(titleSprite->__pos, 1500.0f, tweeny::easing::enumerated::exponentialInOut);
	ptsSprite->TweenScale(glm::vec2(0.0f, 0.0f), 1500.0f, tweeny::easing::enumerated::exponentialInOut);
	titleSprite->TweenScale(glm::vec2(0.954f, 0.954f), 1500.0f, tweeny::easing::enumerated::exponentialInOut);

	std::this_thread::sleep_for(std::chrono::milliseconds(850));
	overlay1->TweenColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	__PAUSE = true;
	spritesToRemove.push_back(overlay1);
	__PAUSE = false;

	StartBBMXScript("scripts/sfst/jeopardy/white.lua");
}

std::vector<SPRITE> Game::GenerateQuestionText(const std::wstring& text, std::shared_ptr<Category>& cat)
{
	std::vector<SPRITE> sprites;
	std::vector<std::wstring> strs;

	std::wstring buf;
	for (wchar_t c : text)
	{
		buf.push_back(c);
		if (fntCards->CalculateWidth(buf, 1.0f) >= 850.0f && c == ' ')
		{
			buf.pop_back();
			strs.push_back(buf);
			buf.clear();
			buf.push_back(c);
		}
	}
	strs.push_back(buf);

	float y = 500.0f;
	for (std::wstring& str : strs)
	{
		float w = fntCards->CalculateWidth(str, 1.0f);
		float x = WIDTH / 2.0f - w / 2.0f;

		SPRITE sprite = MAKE_SPRITE(glm::vec2(x, y), glm::vec2(0.0f, 0.0f), nullptr);
		sprite->font = fntCards;
		sprite->text = str;

		y += 50.0f;
		sprites.push_back(sprite);
	}

	return sprites;
}

void Game::ShowQuestionImage()
{
	if (qImage == nullptr)
	{
		qImage = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f), texOverlayBlack);
	}

	qImage->tex = curQuestion.img;
	qImage->pos = glm::vec2(WIDTH / 2.0f - qImage->tex->width / 2.0f, 500.0f / 2.0f - qImage->tex->height / 2.0f);
	qImage->size = glm::vec2(qImage->tex->width, qImage->tex->height);
	qImage->scale = glm::vec2(curQuestion.imgScale, curQuestion.imgScale);
	qImage->color.a = 0.0f;

	AddSprite(qImage);

	titleSprite->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 750.0f, tweeny::easing::enumerated::linear);
	ptsSprite->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 750.0f, tweeny::easing::enumerated::linear);
	qImage->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f, tweeny::easing::enumerated::linear);

	qImageShown = true;
}

void Game::HideQuestionImage()
{
	titleSprite->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f, tweeny::easing::enumerated::linear);
	ptsSprite->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f, tweeny::easing::enumerated::linear);
	qImage->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 750.0f, tweeny::easing::enumerated::linear);

	std::this_thread::sleep_for(std::chrono::milliseconds(750));

	RemoveSprite(qImage);

	qImageShown = false;
}

void Game::PlayWWMSound(const std::string& snd)
{
	wwmSoundsMap[snd]->Play();
}

void Game::StopWWMSound(const std::string& snd)
{
	wwmSoundsMap[snd]->Stop();
}

void Game::ShowPoints()
{
	__PAUSE = true;
	showingPoints = true;

	AddSprite(overlay1);
	overlay1->color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	overlay1->visible = true;
	overlay1->TweenColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);

	SPRITE ptsTitle = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), nullptr);
	ptsTitle->font = fntCards;
	ptsTitle->text = L"Punkte";
	ptsTitle->scale = glm::vec2(2.0f, 2.0f);
	ptsTitle->pos = glm::vec2(WIDTH / 2.0f - ptsTitle->font->CalculateWidth(ptsTitle->text, 2.0f) / 2.0f, 150.0f);
	AddSprite(ptsTitle);

	SPRITE ptsLine = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(350.0f, 3.0f), texOverlayWhite);
	ptsLine->pos = glm::vec2(WIDTH / 2.0f - ptsLine->size.x / 2.0f, 160.0f);
	AddSprite(ptsLine);

	float longestName = 0.0f;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	for (std::unique_ptr<Contestant>& c : contestants)
	{
		std::wstring str = converter.from_bytes(c->name);
		float length = fntCards->CalculateWidth(str, 1.0f);
		if (length > longestName)
		{
			longestName = length;
		}
	}

	float center = WIDTH / 2.0f;
	int i = 0;
	for (std::unique_ptr<Contestant>& c : contestants)
	{
		std::wstring str = converter.from_bytes(c->name);
		std::wstring str1 = converter.from_bytes(std::to_string(c->points));

		SPRITE s = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), nullptr);
		s->font = fntCards;
		s->text = str;
		s->pos = glm::vec2(center - 35.0f - longestName, 235.0f + 75.0f * i);

		SPRITE s1 = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), nullptr);
		s1->font = fntCards;
		s1->text = str1;
		s1->pos = glm::vec2(center + 35.0f, 235.0f + 75.0f * i);

		AddSprite(s);
		AddSprite(s1);
		ptsSprites.push_back(s);
		ptsSprites.push_back(s1);
		i++;
	}

	ptsSprites.push_back(ptsTitle);
	ptsSprites.push_back(ptsLine);

	for (SPRITE& s : ptsSprites)
	{
		s->color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		s->TweenColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);
	}

	__PAUSE = false;
}

void Game::HidePoints()
{
	__PAUSE = true;

	showingPoints = false;
	for (SPRITE& s : ptsSprites)
	{
		s->TweenColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 1500.0f, tweeny::easing::enumerated::quarticOut);
	}

	__PAUSE = false;
	overlay1->TweenColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 1500.0f, tweeny::easing::enumerated::quarticIn);
	std::this_thread::sleep_for(std::chrono::milliseconds(1800));

	__PAUSE = true;
	overlay1->visible = false;
	overlay1->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	RemoveSprite(overlay1);
	for (SPRITE& s : ptsSprites)
	{
		RemoveSprite(s);
	}
	ptsSprites.clear();
	__PAUSE = false;
}

void Game::StartBBMXScript(const std::string& name)
{
	std::string cmd = "cd bbmx && bbmx.exe -d -r " + name + " -u 60";
	system(cmd.c_str());
}

std::wstring GetAnswer(Game* game)
{
	switch (game->selectedPoints)
	{
	case 0: return game->categories[game->selectedCategoryIdx]->q200.correctAnswer;
	case 1: return game->categories[game->selectedCategoryIdx]->q400.correctAnswer;
	case 2: return game->categories[game->selectedCategoryIdx]->q600.correctAnswer;
	case 3: return game->categories[game->selectedCategoryIdx]->q800.correctAnswer;
	case 4: return game->categories[game->selectedCategoryIdx]->q1000.correctAnswer;
	}
}

void Game::ShowAnswer()
{
	answerShown = true;
	answer = MAKE_SPRITE(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), nullptr);
	std::wstring answerText = GetAnswer(this);
	answer->font = fntCards;
	answer->text = answerText;
	answer->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	float w = fntCards->CalculateWidth(answerText, 1.0f);
	answer->pos.x = WIDTH / 2.0f - w / 2.0f;
	answer->pos.y = HEIGHT - 150.0f;

	AddSprite(answer);
}

void Game::HideAnswer()
{
	answerShown = false;
	RemoveSprite(answer);
}
