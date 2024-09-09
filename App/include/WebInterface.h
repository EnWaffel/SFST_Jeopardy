#pragma once

#include <string>
#include <vector>

class WebInterface
{
public:
	static void Start();
	static void End();
	static void SetContestants(const std::vector<std::string>& names);
	static void UpdatePoints(const std::vector<int>& orderedPoints);
	static void EnableQuestion(const std::wstring& question, const std::wstring& answer);
	static void DisableQuestion();
};