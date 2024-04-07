#include <glad/glad.h>
#include "Manager.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define WIDTH 640
#define HEIGHT 480

Manager::Manager(Game* game) : game(game), shouldExit(false), window(nullptr)
{
}

Manager::~Manager()
{
}

void Manager::Init()
{
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	int monitorsCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);
	GLFWmonitor* monitor;
	if (monitorsCount > 1)
	{
		monitor = monitors[1];
	}
	else
	{
		monitor = monitors[0];
	}

	window = glfwCreateWindow(WIDTH, HEIGHT, "Manager Window", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glfwShowWindow(window);
	
	const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
	int xpos;
	int ypos;
	glfwGetMonitorPos(monitor, &xpos, &ypos);

	glfwSetWindowPos(window, xpos + vidMode->width / 2 - WIDTH / 2, ypos + vidMode->height / 2 - HEIGHT / 2);
	windowCreated = true;

	bool newGameSelected = false;
	std::vector<const char*> contestantNames;
	char newContestantStr[16] = "";

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
		ImGui::Begin("app", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);

		if (ImGui::BeginMenuBar())
		{
			ImGui::MenuItem("Neues Spiel", nullptr, &newGameSelected);
			ImGui::EndMenuBar();
		}

		bool __b = game->gameRunning;
		if (game->gameRunning) ImGui::BeginDisabled();
		if (newGameSelected)
		{
			static int item_current = 1;
			ImGui::ListBox("Kandidaten", &item_current, contestantNames.data(), contestantNames.size(), 3);

			ImGui::SetNextItemWidth(200);
			ImGui::InputText("Neuer Kandidat", newContestantStr, IM_ARRAYSIZE(newContestantStr));
			if (ImGui::Button(u8"Kandidat hinzufügen"))
			{
				if (!newContestantStr[0] == '\0')
				{
					char* str = new char[16];
					strcpy_s(str, 16, newContestantStr);
					contestantNames.push_back(str);
				}
			}

			if (contestantNames.size() < 3)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"Es müssen mindestens 3 Kandidaten mit machen!");
				ImGui::BeginDisabled();
			}
			if (ImGui::Button("Spiel Starten!"))
			{
				std::vector<std::string> vec;
				for (const char* s : contestantNames)
				{
					vec.push_back(s);
				}
				game->NewGame(vec);
			}
			if (contestantNames.size() < 3)
			{
				ImGui::EndDisabled();
			}
		}
		if (game->gameRunning && __b) ImGui::EndDisabled();

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwSetWindowShouldClose(game->window, GLFW_TRUE);

	for (const char* str : contestantNames)
	{
		delete[] str;
	}
}

void Manager::Terminate()
{
}
