#pragma once
#include "Game.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class Manager
{
public:
	bool shouldExit;
	Game* game;
	GLFWwindow* window;
	bool windowCreated = false;
public:
	Manager(Game* game);
	~Manager();

	void Init();
	void Terminate();
};