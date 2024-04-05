#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader
{
private:
	unsigned int programId;
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Use();
	void SetMatrix(const std::string& name, const glm::mat4& mat);
	void SetVec4(const std::string& name, glm::vec4 vec);
	void SetVec2(const std::string& name, glm::vec2 vec);
	void SetBool(const std::string& name, bool b);
	void SetFloat(const std::string& name, float f);
	void SetInt(const std::string& name, int i);
};