#include "Category.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Logger.h"
using json = nlohmann::json;

static std::vector<std::string> ArrayToVec(json& j)
{
	std::vector<std::string> vec;
	for (json::iterator it = j.begin(); it != j.end(); ++it)
	{
		vec.push_back(it.value());
	}
	return vec;
}

Category::Category()
{
}

Category::~Category()
{
}

std::shared_ptr<Category> Category::LoadFromFile(const std::string& path)
{
	std::ifstream s(path);
	json data = json::parse(s);
	s.close();

	std::shared_ptr<Category> c = std::make_shared<Category>();
	std::string title = data["title"];
	c->title = title;

	c->q200 = {
		data["200"]["question"],
		data["200"]["correct_answer"],
		ArrayToVec(data["200"]["answers"])
	};

	c->q400 = {
		data["400"]["question"],
		data["400"]["correct_answer"],
		ArrayToVec(data["400"]["answers"])
	};

	c->q600 = {
		data["600"]["question"],
		data["600"]["correct_answer"],
		ArrayToVec(data["600"]["answers"])
	};

	c->q800 = {
		data["800"]["question"],
		data["800"]["correct_answer"],
		ArrayToVec(data["800"]["answers"])
	};
	
	c->q1000 = {
		data["1000"]["question"],
		data["1000"]["correct_answer"],
		ArrayToVec(data["1000"]["answers"])
	};

	c->excludedQuestions = ArrayToVec(data["excluded_points"]);

	LogInfo("Loaded Category: " + title);
	return c;
}
