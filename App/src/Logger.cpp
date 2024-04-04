#include "Logger.h"
#include <iostream>
#include <fstream>
#include <cstdio>

static std::ofstream s;

void InitLog()
{
	remove("log.txt");
	s = std::ofstream("log.txt", std::ios_base::out);
}

void SaveLog()
{
	s.close();
}

void LogInfo(const std::string& msg)
{
	std::string str = "[INFO]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
}

void LogWarning(const std::string& msg)
{
	std::string str = "[WARNING]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
}

void LogError(const std::string& msg)
{
	std::string str = "[ERROR]: " + msg;
	std::cout << str << std::endl;
	s << str << '\n';
}
