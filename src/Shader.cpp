#include "Shader.h"
#include "Utilities.h"

Shader::Shader(const std::string& filepath):
	m_FilePath(filepath)
{
	ShaderSources source = ParseShader(m_FilePath);
	std::cout << "Vertex Shader" << std::endl;
	std::cout << source.VertexShader << std::endl;
	std::cout << "Fragment Shader" << std::endl;
	std::cout << source.FragmentShader << std::endl; 

	m_RenderID = CreateShaderProgram(source.VertexShader, source.FragmentShader);
}

Shader::~Shader(){
	GLCall(glDeleteProgram(m_RenderID));
}

void Shader::Bind() const{
	GLCall(glUseProgram(m_RenderID));
}

void Shader::Unbind() const{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int v0){
	GLCall(glUniform1i(GetUniformLocation(name), v0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3){
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix){
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

ShaderSources Shader::ParseShader(const std::string& filepath){
	std::ifstream stream(filepath);

	enum class ShaderType{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while(std::getline(stream, line)){
		if(line.find("#shader") != std::string::npos){
			if(line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if(line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else{
			ss[(int) type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompilerShader(unsigned int type, const std::string& source){
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE){
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "failed to compiler " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
	}

	return id;
}

unsigned int Shader::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource){
	unsigned int program = glCreateProgram();
	unsigned int vs = CompilerShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = CompilerShader(GL_FRAGMENT_SHADER, fragmentSource);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
	if(m_UniformLocations.find(name) != m_UniformLocations.end())
		return m_UniformLocations[name];

	GLCall(int location = glGetUniformLocation(m_RenderID, name.c_str()));
	if(location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

	m_UniformLocations[name] = location;
	return location;
}
