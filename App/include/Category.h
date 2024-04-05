#pragma once
#include <memory>
#include <string>
#include <vector>

struct Question
{
	std::string q;
	std::string correctAnswer;
	std::vector<std::string> answers;
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