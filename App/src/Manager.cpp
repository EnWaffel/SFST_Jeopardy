#include <glad/glad.h>
#include "Manager.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Category.h"
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;

extern bool __REQ_SYNC;
extern bool __SYNC;
static Game* _game;

#define WIDTH 640
#define HEIGHT 740

static std::vector<std::string> SeparatedStrToVec(const std::string& str)
{
	std::vector<std::string> vec;
	std::string buf;
	for (char c : str)
	{
		if (c == ',')
		{
			vec.push_back(buf);
			buf.clear();
			continue;
		}
		buf.push_back(c);
	}
	vec.push_back(buf);
	return vec;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	try {
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_PAGE_DOWN)
			{
				if (_game->state == COUNTDOWN_OUT)
				{
					_game->Next(COUNTDOWN_OUT, nullptr);
				}
				else if (_game->state == REVEAL_CATEGORIES)
				{
					_game->Next(REVEAL_CATEGORIES, nullptr);
				}
			}
			else if (key == GLFW_KEY_1)
			{
				int i = 0;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_2)
			{
				int i = 1;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_3)
			{
				int i = 2;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_4)
			{
				int i = 3;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_5)
			{
				int i = 4;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_6)
			{
				int i = 5;
				_game->Next(AWAIT_BUZZ_CONFIRM, &i);
			}
			else if (key == GLFW_KEY_UP)
			{
				_game->Next(AWAIT_BUZZ, nullptr);
			}
		}
	}
	catch (std::exception e)
	{
		LogError(std::string("Manager::KeyCallback(): Caught exception: ") + e.what());
	}
}

Manager::Manager(Game* game) : game(game), shouldExit(false), window(nullptr)
{
	_game = game;
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

	glfwSetKeyCallback(window, KeyCallback);

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

	bool newGameSelected = true;
	bool createCategorySelected = false;
	bool editCategorySelected = false;
	std::vector<std::string> contestantNames;
	char newContestantStr[17] = "";
	char cateName[64] = "";
	char excludedPoints[64] = "";

	char q200Name[512] = "";
	char q200correctAnswer[512] = "";

	char q400Name[512] = "";
	char q400correctAnswer[512] = "";

	char q600Name[512] = "";
	char q600correctAnswer[512] = "";

	char q800Name[512] = "";
	char q800correctAnswer[512] = "";

	char q1000Name[512] = "";
	char q1000correctAnswer[512] = "";

	int selectedCategory = 0;
	int selectedPoints = 0;

	contestantNames.push_back("Team1");
	contestantNames.push_back("Team2");
	contestantNames.push_back("Team3");
	contestantNames.push_back("Team4");
	contestantNames.push_back("Team5");
	contestantNames.push_back("Team6");

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
			if (ImGui::MenuItem("Neues Spiel", nullptr, &newGameSelected))
			{
				createCategorySelected = false;
				editCategorySelected = false;
			}
			if (ImGui::BeginMenu("Kategorie Manager"))
			{
				if (ImGui::MenuItem("Kategorie Erstellen", nullptr, &createCategorySelected))
				{
					editCategorySelected = false;
					newGameSelected = false;
				}
				if (ImGui::MenuItem("Kategorie Bearbeiten", nullptr, &editCategorySelected))
				{
					createCategorySelected = false;
					newGameSelected = false;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		bool __b = false;
		if (game->gameRunning)
		{
			__b = true;
			ImGui::BeginDisabled();
		}
		if (newGameSelected)
		{
			static int item_current = 1;
			ImGui::BeginListBox("Kandidaten");
			std::vector<std::string> toRemove;

			for (std::string& s : contestantNames)
			{
				bool selected = false;
				ImGui::Selectable(s.c_str(), &selected);
				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						toRemove.push_back(s);
					}
				}
			}
			for (std::string& s : toRemove)
			{
				contestantNames.erase(std::find(contestantNames.begin(), contestantNames.end(), s));
			}

			ImGui::EndListBox();

			ImGui::SetNextItemWidth(200);
			ImGui::InputText("Neuer Kandidat", newContestantStr, IM_ARRAYSIZE(newContestantStr));
			if (ImGui::Button(u8"Kandidat hinzufügen"))
			{
				if (!newContestantStr[0] == '\0')
				{
					if (std::find(contestantNames.begin(), contestantNames.end(), std::string(newContestantStr)) == contestantNames.end())
					{
						contestantNames.push_back(newContestantStr);
					}
				}
			}

			if (contestantNames.size() < 2)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"Es müssen mehr oder 6 Teams mitmachen!");
				ImGui::BeginDisabled();
			}
			if (ImGui::Button("Spiel Starten!"))
			{
				__REQ_SYNC = true;
				while (!__SYNC);
				__SYNC = false;
				try {
					game->NewGame(contestantNames);
				}
				catch (std::exception e)
				{
					LogError(std::string("Game::NewGame(): Caught exception: ") + e.what());
				}
			}
			if (contestantNames.size() < 2)
			{
				ImGui::EndDisabled();
			}
			if (game->gameRunning && __b)
			{
				ImGui::EndDisabled();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			bool ___b = !game->gameRunning || game->state != PLAYER_CAT_SELECT;
			if (!game->gameRunning || game->state != PLAYER_CAT_SELECT)
			{
				___b = true;
				ImGui::BeginDisabled();
			}

			ImGui::Text("Kategorie:");

			for (int i = 0; i < 5; i++)
			{
				ImGui::SameLine();
				bool b0 = false;
				
				if (selectedCategory == i)
				{
					b0 = true;
					ImGui::SetWindowFontScale(1.35f);
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				std::string text = " ";
				text.append(std::to_string(i + 1));
				text.append(" ");

				if (ImGui::Button(text.c_str()))
				{
					selectedCategory = i;
				}

				if (b0)
				{
					ImGui::PopStyleColor();
					ImGui::SetWindowFontScale(1.0f);
				}
			}

			ImGui::Text("Punkte:");

			for (int i = 0; i < 5; i++)
			{
				ImGui::SameLine();
				bool b0 = false;

				if (selectedPoints == i)
				{
					b0 = true;
					ImGui::SetWindowFontScale(1.35f);
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				std::string text;

				switch (i)
				{
				case 0: { text = "200"; break; }
				case 1: { text = "400"; break; }
				case 2: { text = "600"; break; }
				case 3: { text = "800"; break; }
				case 4: { text = "1000"; break; }
				}

				if (ImGui::Button(text.c_str()))
				{
					selectedPoints = i;
				}

				if (b0)
				{
					ImGui::PopStyleColor();
					ImGui::SetWindowFontScale(1.0f);
				}
			}

			ImGui::SetWindowFontScale(1.5f);
			if (ImGui::Button(u8"Auswählen"))
			{
				game->selectedCategoryIdx = selectedCategory;
				game->selectedPoints = selectedPoints;
				game->Next(PLAYER_CAT_SELECT, nullptr);
			}
			ImGui::SetWindowFontScale(1.0f);

			if ((!game->gameRunning || game->state != PLAYER_CAT_SELECT) && ___b)
			{
				ImGui::EndDisabled();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (_game->showingPoints)
			{
				if (ImGui::Button("Punkte verbergen"))
				{
					_game->HidePoints();
				}
			}
			else
			{
				if (ImGui::Button("Punkte zeigen"))
				{
					_game->ShowPoints();
				}
			}

			ImGui::Spacing();

			bool ____b = !game->gameRunning || game->state != AWAIT_CORRECT_OR_WRONG;
			if (!game->gameRunning || game->state != AWAIT_CORRECT_OR_WRONG)
			{
				____b = true;
				ImGui::BeginDisabled();
			}

			ImGui::SetWindowFontScale(2.0f);
			if (ImGui::Button("Richtig"))
			{
				bool v = true;
				game->Next(AWAIT_CORRECT_OR_WRONG, &v);
			}
			ImGui::SameLine();
			if (ImGui::Button("Falsch"))
			{
				bool v = false;
				game->Next(AWAIT_CORRECT_OR_WRONG, &v);
			}
			ImGui::SetWindowFontScale(1.0f);

			if ((!game->gameRunning || game->state != AWAIT_CORRECT_OR_WRONG) && ____b)
			{
				ImGui::EndDisabled();
			}
			
			if (game->gameRunning && (game->state == AWAIT_CORRECT_OR_WRONG || game->state == AWAIT_BUZZ || game->state == AWAIT_BUZZ_CONFIRM)) {
				ImGui::SetWindowFontScale(1.5f);
				ImGui::Text("Antwort:");
				ImGui::SetWindowFontScale(1.15f);
				ImGui::Text(game->curQuestion._correctAnswer.c_str());
				ImGui::SetWindowFontScale(1.0f);

				if (game->curQuestion.hasImage)
				{
					if (game->qImageShown)
					{
						if (ImGui::Button("Bild verbergen"))
						{
							game->HideQuestionImage();
						}
					}
					else
					{
						if (ImGui::Button("Bild zeigen"))
						{
							game->ShowQuestionImage();
						}
					}
				}
			}
		}

		if (game->gameRunning && game->state == COUNTDOWN_OUT)
		{
			if (game->answerShown)
			{
				if (ImGui::Button("Antwort verbergen"))
				{
					game->HideAnswer();
				}
			}
			else
			{
				if (ImGui::Button("Antwort zeigen"))
				{
					game->ShowAnswer();
				}
			}
		}

		if (createCategorySelected)
		{
			ImGui::SetNextItemWidth(380);
			ImGui::InputText("Kategorie Name", cateName, IM_ARRAYSIZE(cateName));
			ImGui::SetNextItemWidth(220);
			ImGui::InputText("Ausgenommene Punkte (getrennt mit ',')", excludedPoints, IM_ARRAYSIZE(excludedPoints));

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.35f);
			ImGui::TextColored(ImVec4(0.0f, 0.459f, 0.02f, 1.0f), "200 Punkte Frage:");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(111);
			ImGui::InputText("Frage", q200Name, IM_ARRAYSIZE(q200Name));
			ImGui::PopID();
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(113);
			ImGui::InputText("Richtige Antwort", q200correctAnswer, IM_ARRAYSIZE(q200correctAnswer));
			ImGui::PopID();

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.35f);
			ImGui::TextColored(ImVec4(0.0f, 0.459f, 0.02f, 1.0f), "400 Punkte Frage:");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(114);
			ImGui::InputText("Frage", q400Name, IM_ARRAYSIZE(q400Name));
			ImGui::PopID();
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(116);
			ImGui::InputText("Richtige Antwort", q400correctAnswer, IM_ARRAYSIZE(q400correctAnswer));
			ImGui::PopID();

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.35f);
			ImGui::TextColored(ImVec4(0.0f, 0.459f, 0.02f, 1.0f), "600 Punkte Frage:");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(117);
			ImGui::InputText("Frage", q600Name, IM_ARRAYSIZE(q600Name));
			ImGui::PopID();
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(119);
			ImGui::InputText("Richtige Antwort", q600correctAnswer, IM_ARRAYSIZE(q600correctAnswer));
			ImGui::PopID();

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.35f);
			ImGui::TextColored(ImVec4(0.0f, 0.459f, 0.02f, 1.0f), "800 Punkte Frage:");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(1110);
			ImGui::InputText("Frage", q800Name, IM_ARRAYSIZE(q800Name));
			ImGui::PopID();
			ImGui::SetNextItemWidth(220); 
			ImGui::PushID(1112);
			ImGui::InputText("Richtige Antwort", q800correctAnswer, IM_ARRAYSIZE(q800correctAnswer));
			ImGui::PopID();

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.35f);
			ImGui::TextColored(ImVec4(0.0f, 0.459f, 0.02f, 1.0f), "1000 Punkte Frage:");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(1113);
			ImGui::InputText("Frage", q1000Name, IM_ARRAYSIZE(q1000Name));
			ImGui::PopID();
			ImGui::SetNextItemWidth(220);
			ImGui::PushID(1115);
			ImGui::InputText("Richtige Antwort", q1000correctAnswer, IM_ARRAYSIZE(q1000correctAnswer));
			ImGui::PopID();

			ImGui::Separator();
			ImGui::SetWindowFontScale(1.15f);
			if (ImGui::Button("Kategorie Erstellen"))
			{
				json data;
				data["title"] = std::string(cateName);
				data["excluded_points"] = SeparatedStrToVec(std::string(excludedPoints));

				data["200"]["question"] = std::string(q200Name);
				data["200"]["correct_answer"] = std::string(q200correctAnswer);

				data["400"]["question"] = std::string(q400Name);
				data["400"]["correct_answer"] = std::string(q400correctAnswer);

				data["600"]["question"] = std::string(q600Name);
				data["600"]["correct_answer"] = std::string(q600correctAnswer);

				data["800"]["question"] = std::string(q800Name);
				data["800"]["correct_answer"] = std::string(q800correctAnswer);

				data["1000"]["question"] = std::string(q1000Name);
				data["1000"]["correct_answer"] = std::string(q1000correctAnswer);

				std::ofstream o("assets/categories/" + std::string(cateName) + ".json");
				o << std::setw(4) << data << std::endl;
			}
			ImGui::SetWindowFontScale(1.0f);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwSetWindowShouldClose(game->window, GLFW_TRUE);
}

void Manager::Terminate()
{
}
