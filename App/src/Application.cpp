#include <AL/al.h>
#include <AL/alc.h>
#include "Application.h"
#include "Game.h"
#include "Manager.h"
#include "Logger.h"
#include <memory>
#include <GLFW/glfw3.h>
#include <sstream>
#include <thread>
#include "Serial.h"
#include "PortScanner.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <ini.h>
#include "Sound.h"

static ALCdevice* device;
static ALCcontext* context;

int Application::Init()
{

    SetConsoleOutputCP(CP_UTF8);
    InitLog();

    std::vector<std::string> ports = PortScanner::GetAllPorts();
    if (ports.size() < 1)
    {
        LogError("huh.... keine COM Ports? Da stimmt was nicht.");

#ifndef _DEBUG
        std::cin.get();
#endif

        SaveLog();
        return -1;
    }

    mINI::INIFile file("startup.ini");
    mINI::INIStructure ini;
    file.read(ini);

    Sound::SetVolume(std::atof(ini["settings"]["volume"].c_str()));

    bool askForPort = ini["serial"]["ask_port"] == "true" ? true : false;
    bool wwmSounds = ini["additions"]["werwirdmillionair"] == "true" ? true : false;
    std::string port;
    if (!askForPort)
    {
        port = ini["serial"]["port"];
    }

    if (port.empty())
    {
        std::cout << "Zu welchem COM Port ist der ESP / Controller verbunden?\n";
        std::cout << "COM Ports:" << std::endl;
        for (std::string& s : ports)
        {
            std::cout << "- " << s << std::endl;
        }
        std::cout << "> ";

        std::cin >> port;
    }

    if (std::find(ports.begin(), ports.end(), port) == ports.end())
    {
        std::cout << "'" << port << u8"' ist nicht verfügbar!" << std::endl;

#ifndef _DEBUG
        std::cin.get();
#endif

        SaveLog();
        return -1;
    }
    
    std::cout << "COM Port: " << port << " wird verwendet!\n" << std::endl;

    std::unique_ptr<Serial> ser = std::make_unique<Serial>(port);
    if (!ser->Open())
    {
        LogError("Couldn't connect to controller!");
        SaveLog();
        return -1;
    }

    uint8_t __buf[2] = {1, SERIAL_ACTION_CHECK};
    ser->Write(__buf, sizeof(__buf));

    unsigned long long start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    uint8_t ___buf = 0;
    do
    {
        unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if (now >= start + 5000)
        {
            LogError("Test signal timeout!");
            ser->Close();
            SaveLog();
            return -1;
        }

        ser->Read(&___buf, sizeof(___buf));
    } while (___buf != '!');

    uint8_t _buf[3];
    ser->Read(_buf, sizeof(_buf));

    if (!(_buf[0] == 'h' && _buf[1] == 'i' && _buf[2] == '!'))
    {
        LogError("Received invalid test signal!");
        ser->Close();
        SaveLog();
        return -1;
    }

    SetLogColor(COLOR_FOREGROUND_GREEN);
    LogInfo("Test signal received!");
    ResetLogColor();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (glfwInit() == GLFW_FALSE)
    {
        LogError("Failed to initialize GLFW!");
        SaveLog();
        return -1;
    }

    LogInfo("Initializing OpenAL...");
    device = alcOpenDevice(nullptr);
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    Game* game = new Game;
    game->wwmSounds = wwmSounds;
    game->ser = std::move(ser);
    game->quickAnims = ini["settings"]["quick_anims"] == "true" ? true : false;

    Manager* manager = new Manager(game);

    std::thread thd([&](){
        manager->Init();
    });
    
    while (!manager->windowCreated)
    {
        Sleep(1);
    }

    bool running = true;
    std::thread readThd([&]() {
        while (running)
        {
            if (game->state != AWAIT_BUZZ) continue;

            uint8_t startByte = 0;
            game->ser->Read(reinterpret_cast<uint8_t*>(&startByte), sizeof(startByte));
            if (startByte == 1)
            {
                uint8_t action = 0;
                game->ser->Read(&action, sizeof(uint8_t));

                switch (action)
                {
                case SERIAL_ACTION_BTN_ACT1:
                {
                    game->Next(AWAIT_BUZZ, nullptr);
                    break;
                }
                }
            }
        }
    });

    int gameResult;
    if ((gameResult = game->Init()) != 0)
    {
        std::stringstream ss;
        ss << std::hex << gameResult;
        LogError("Failed to initialize Game Window! (Error Code: " + ss.str() + ")");
        glfwTerminate();
        return -1;
    }

    LogInfo("Exiting...");

    running = false;
    glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    thd.join();
    readThd.join();
    manager->Terminate();
    game->Terminate();
    delete game;
    delete manager;

    alcCloseDevice(device);
    alcDestroyContext(context);
    glfwTerminate();
    SaveLog();

    LogInfo("Goodbye! :)");
    return 0;
}
