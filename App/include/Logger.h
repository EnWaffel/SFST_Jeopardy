#pragma once
#include <string>

#define COLOR_FOREGROUND_BLUE      0x0001 // text color contains blue.
#define COLOR_FOREGROUND_GREEN     0x0002 // text color contains green.
#define COLOR_FOREGROUND_RED       0x0004 // text color contains red.
#define COLOR_FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define COLOR_BACKGROUND_BLUE      0x0010 // background color contains blue.
#define COLOR_BACKGROUND_GREEN     0x0020 // background color contains green.
#define COLOR_BACKGROUND_RED       0x0040 // background color contains red.
#define COLOR_BACKGROUND_INTENSITY 0x0080 // background color is intensified.

void InitLog();
void SaveLog();

void SetLogColor(unsigned short color);
void ResetLogColor();

void LogInfo(const std::string& msg);
void LogWarning(const std::string& msg);
void LogError(const std::string& msg);