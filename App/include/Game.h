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
#include "Serial.h"
#include "Telemetry.h"

#define BOARD_CATEGORIES 5

#define GAME_ERROR_GLFW_INIT 0x10

enum State : int8_t
{
	NONE,
	REVEAL_CATEGORIES,
	PLAYER_NEXT,
	PLAYER_CAT_SELECT,
	AWAIT_BUZZ,
	AWAIT_BUZZ_CONFIRM,
	AWAIT_CORRECT_OR_WRONG,
	COUNTDOWN_OUT
};

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
	TEXTURE texBGEmpty;
	TEXTURE texOverlayBlack;
	TEXTURE texJoker;
	TEXTURE texVignetteCorrect;
	TEXTURE texVignetteWrong;
	TEXTURE texOverlayWhite;
	FONT fntCards;
	SOUND sndBoardFill;
	SOUND sndBuzzer;
	SPRITE emptyBG;
	SPRITE overlay1;
	SPRITE joker;
	SPRITE vignette1;
	SPRITE countdownText;
	SPRITE qImage = nullptr;
	std::vector<std::shared_ptr<Category>> loadedCategories;
	std::vector<std::shared_ptr<Category>> categories;
	std::vector<std::unique_ptr<Contestant>> contestants;
	int curSelectingContestant = 0;
	std::vector<SPRITE> cateSprites;
	std::map<std::string, TEXTURE> categoryTitleTextures;
	bool gameRunning = false;
	int catesRevealed = 0;
	std::unique_ptr<Serial> ser;
	State state = NONE;
	int selectedCategoryIdx;
	int selectedPoints;
	bool jokerAnim;
	int buzzedContestantIdx;
	bool correctGuess = false;
	int guessingContestant = 0;
	float countdown;
	bool countdownActive = false;
	std::vector<SPRITE> textSprites;
	std::vector<SPRITE> spritesToRemove;
	SPRITE titleSprite;
	SPRITE ptsSprite;
	Question curQuestion;
	bool qImageShown = false;
	int winPts = 0;
	Telemetry tm;
	bool wwmSounds = false;
	std::map<std::string, SOUND> wwmSoundsMap;
	bool quickAnims = false;
	std::vector<SPRITE> ptsSprites;
	bool showingPoints = false;
	bool answerShown = false;
	SPRITE answer = nullptr;
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
	void PlayBoardFillAnimation();
	void SelectCategories();
	void CreateGameBoard();
	void DrawSprites();
	void DrawTextSprite(SPRITE& sprite);
	void RevealNextCategory();
	void Next(State targetState, void* data);
	std::string StateToString(const State& state);
	void SelectCatAndPoints();
	void DeselectCatAndPoints();
	std::vector<SPRITE> GenerateQuestionText(const std::wstring& text, std::shared_ptr<Category>& cat);
	void ShowQuestionImage();
	void HideQuestionImage();
	void PlayWWMSound(const std::string& snd);
	void StopWWMSound(const std::string& snd);
	void ShowPoints();
	void HidePoints();
	void StartBBMXScript(const std::string& name);
	void ShowAnswer();
	void HideAnswer();
};
