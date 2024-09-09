#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Texture.h"

struct Question
{
	std::wstring q;
	std::string _correctAnswer;
	std::wstring correctAnswer;
	bool isJoker;
	std::shared_ptr<Texture> img = nullptr;
	float imgScale = 1.0f;
	bool hasImage;
};

class Category
{
public:
	std::string title;
	Question q200;
	Question q400;
	Question q600;
	Question q800;
	Question q1000;
	std::vector<std::string> excludedQuestions;
public:
	Category();
	~Category();

	static std::shared_ptr<Category> LoadFromFile(const std::string& path);
};