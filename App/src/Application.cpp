#include <AL/al.h>
#include <AL/alc.h>
#include "Application.h"
#include "Game.h"
#include "Manager.h"
#include <memory>
#include "Logger.h"
#include <GLFW/glfw3.h>
#include <sstream>
#include <thread>

static ALCdevice* device;
static ALCcontext* context;

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

    LogInfo("Initializing OpenAL...");
    device = alcOpenDevice(nullptr);
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);


    Game* game = new Game;
    Manager* manager = new Manager(game);

    std::thread thd([&](){
        manager->Init();
    });
    
    while (!manager->windowCreated);

    int gameResult;
    if ((gameResult = game->Init()) != 0)
    {
        std::stringstream ss;
        ss << std::hex << gameResult;
        LogError("Failed to initialize Game Window! (Error Code: " + ss.str() + ")");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    thd.join();
    manager->Terminate();
    game->Terminate();
    delete game;
    delete manager;

    alcCloseDevice(device);
    alcDestroyContext(context);
    glfwTerminate();
    SaveLog();
    return 0;
}
