#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"

struct ShaderSources{
	std::string VertexShader;
	std::string FragmentShader;
};

class Shader{
private:
	unsigned int	m_RenderID;
	std::string		m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocations;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int v0);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
	ShaderSources ParseShader(const std::string& filepath);
	unsigned int CompilerShader(unsigned int type, const std::string& source);
	unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

	int GetUniformLocation(const std::string& name);
};