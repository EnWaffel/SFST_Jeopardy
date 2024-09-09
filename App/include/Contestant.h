#pragma once
#include <string>

class Contestant
{
public:
	int points;
	std::string name;
public:
	Contestant(const std::string& name);
	~Contestant();
};