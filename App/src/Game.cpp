#include "Game.h"
#include "Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#define WIDTH 1280.0f
#define HEIGHT 720.0f

static GLFWwindow* window;

int Game::Init()
{
	LogInfo("Initializing Game...");

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

	glfwShowWindow(window);

	Run();
	return 0;
}

void Game::Terminate()
{
	LogInfo("Disabling Game...");
	
	glfwDestroyWindow(window);

	LogInfo("Game disabled!");
}

void Game::Run()
{
	LogInfo("Running Game!");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}
