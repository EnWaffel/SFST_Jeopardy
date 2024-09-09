#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read shader files: " << e.what() << std::endl;
	}

	int success;
	char infoLog[1024];

	const char* _vertexCode = vertexCode.c_str();
	const char* _fragmentCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &_vertexCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
		std::cout << "Failed to compile vertex shader: " << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &_fragmentCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
		std::cout << "Failed to compile fragment shader: " << infoLog << std::endl;
	}

	programId = glCreateProgram();
	glAttachShader(programId, vertex);
	glAttachShader(programId, fragment);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, 1024, NULL, infoLog);
		std::cout << "Failed to link shader program: " << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(programId);
}

void Shader::Use()
{
	glUseProgram(programId);
}

void Shader::SetMatrix(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetVec4(const std::string& name, glm::vec4 vec)
{
	glUniform4f(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetVec2(const std::string& name, glm::vec2 vec)
{
	glUniform2f(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y);
}

void Shader::SetBool(const std::string& name, bool b)
{
	glUniform1i(glGetUniformLocation(programId, name.c_str()), b);
}

void Shader::SetFloat(const std::string& name, float f)
{
	glUniform1f(glGetUniformLocation(programId, name.c_str()), f);
}

void Shader::SetInt(const std::string& name, int i)
{
	glUniform1i(glGetUniformLocation(programId, name.c_str()), i);
}