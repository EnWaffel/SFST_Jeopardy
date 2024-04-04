#include "Application.h"
#include "Game.h"
#include "Manager.h"
#include <memory>
#include "Logger.h"
#include <GLFW/glfw3.h>
#include <sstream>

int Application::Init()
{
    InitLog();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (glfwInit() == GLFW_FALSE)
    {
        LogError("Failed to initialize GLFW!");
        return -1;
    }
    
    int gameResult;
    if ((gameResult = Game::Init()) != 0)
    {
        std::stringstream ss;
        ss << std::hex << gameResult;
        LogError("Failed to initialize Game Window! (Error Code: " + ss.str() + ")");
        glfwTerminate();
        return -1;
    }

    Game::Terminate();

    glfwTerminate();
    SaveLog();
    return 0;
}
