#include "Logger.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <Windows.h>

static std::ofstream s;
static HANDLE hConsole;

void InitLog()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	remove("log.txt");
	s = std::ofstream("log.txt", std::ios_base::out);
}

void SaveLog()
{
	s.close();
}

void SetLogColor(unsigned short color)
{
	SetConsoleTextAttribute(hConsole, color);
}

void ResetLogColor()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void LogInfo(const std::string& msg)
{
	std::string str = "[INFO]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
	ResetLogColor();
}

void LogWarning(const std::string& msg)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	std::string str = "[WARNING]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
	ResetLogColor();
}

void LogError(const std::string& msg)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	std::string str = "[ERROR]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
	ResetLogColor();
}
