#pragma once
#include <string>

class Telemetry
{
public:
	int correctGuesses = 0;
	int wrongGuesses = 0;
	int contestants = 0;
public:
	Telemetry();
	~Telemetry();

	void Save();
};
