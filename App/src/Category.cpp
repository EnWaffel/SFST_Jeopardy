#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "Category.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Logger.h"
#include <iostream>
#include <codecvt>
#include "Defs.h"
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

static std::wstring ToWStr(const std::string& s)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring str = converter.from_bytes(s);
	return str;
}

Category::Category()
{
}

Category::~Category()
{
}

std::shared_ptr<Category> Category::LoadFromFile(const std::string& path)
{
	std::ifstream s(path, std::ios::binary);
	json data = json::parse(s);
	s.close();

	std::shared_ptr<Category> c = std::make_shared<Category>();
	std::string title = data["title"];
	c->title = title;

	c->q200 = {
		ToWStr(data["200"]["question"]),
		data["200"]["correct_answer"],
		ToWStr(data["200"]["correct_answer"])
	};
	if (data["200"].contains("img"))
	{
		c->q200.img = MAKE_TEXlog(std::string("assets/images/") + std::string(data["200"]["img"]));
		c->q200.hasImage = true;
	}
	if (data["200"].contains("scale"))
	{
		c->q200.imgScale = data["200"]["scale"];
	}

	c->q400 = {
		ToWStr(data["400"]["question"]),
		data["400"]["correct_answer"],
		ToWStr(data["400"]["correct_answer"])
	};
	if (data["400"].contains("img"))
	{
		c->q400.img = MAKE_TEXlog(std::string("assets/images/") + std::string(data["400"]["img"]));
		c->q400.hasImage = true;
	}
	if (data["400"].contains("scale"))
	{
		c->q400.imgScale = data["400"]["scale"];
	}

	c->q600 = {
		ToWStr(data["600"]["question"]),
		data["600"]["correct_answer"],
		ToWStr(data["600"]["correct_answer"])
	};
	if (data["600"].contains("img"))
	{
		c->q600.img = MAKE_TEXlog(std::string("assets/images/") + std::string(data["600"]["img"]));
		c->q600.hasImage = true;
	}
	if (data["600"].contains("scale"))
	{
		c->q600.imgScale = data["600"]["scale"];
	}

	c->q800 = {
		ToWStr(data["800"]["question"]),
		data["800"]["correct_answer"],
		ToWStr(data["800"]["correct_answer"])
	};
	if (data["800"].contains("img"))
	{
		c->q800.img = MAKE_TEXlog(std::string("assets/images/") + std::string(data["800"]["img"]));
		c->q800.hasImage = true;
	}
	if (data["800"].contains("scale"))
	{
		c->q800.imgScale = data["800"]["scale"];
	}
	
	c->q1000 = {
		ToWStr(data["1000"]["question"]),
		data["1000"]["correct_answer"],
		ToWStr(data["1000"]["correct_answer"])
	};
	if (data["1000"].contains("img"))
	{
		c->q1000.img = MAKE_TEXlog(std::string("assets/images/") + std::string(data["1000"]["img"]));
		c->q1000.hasImage = true;
	}
	if (data["1000"].contains("scale"))
	{
		c->q1000.imgScale = data["1000"]["scale"];
	}

	c->q200.isJoker = c->q200.q == L"JOKER";
	c->q400.isJoker = c->q400.q == L"JOKER";
	c->q600.isJoker = c->q600.q == L"JOKER";
	c->q800.isJoker = c->q800.q == L"JOKER";
	c->q1000.isJoker = c->q1000.q == L"JOKER";

	c->excludedQuestions = ArrayToVec(data["excluded_points"]);

	LogInfo("Loaded Category: " + std::string(title.begin(), title.end()));
	return c;
}
