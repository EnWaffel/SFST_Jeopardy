#pragma once

#define GAME_ERROR_GLFW_INIT 0x10

class Game
{
public:
	static int Init();
	static void Terminate();

	static void Run();
};