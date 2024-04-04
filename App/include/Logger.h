#pragma once
#include <string>

void InitLog();
void SaveLog();

void LogInfo(const std::string& msg);
void LogWarning(const std::string& msg);
void LogError(const std::string& msg);