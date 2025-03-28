#include "glpch.h"
#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>

static std::string ReadFileAsString(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize((size_t)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else
	{
		LOG_ERROR("Could not open file '{0}'", filepath);
	}

	return result;
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

GLuint Shader::CompileShader(GLenum type, const std::string& source)
{
	GLuint shader = glCreateShader(type);

	const GLchar* sourceCStr = source.c_str();
	glShaderSource(shader, 1, &sourceCStr, 0);

	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shader);

		LOG_ERROR("{0}", infoLog.data());
		// HZ_CORE_ASSERT(false, "Shader compilation failure!");
	}

	return shader;
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	LoadFromGLSLTextFiles(vertexShaderPath, fragmentShaderPath);
}

void Shader::LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::string vertexSource = ReadFileAsString(vertexShaderPath);
	std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

	GLuint program = glCreateProgram();
	int glShaderIDIndex = 0;

	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	glAttachShader(program, vertexShader);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(program);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		LOG_ERROR("{0}", infoLog.data());
		// HZ_CORE_ASSERT(false, "Shader link failure!");
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_RendererID = program;
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::UnBind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const std::string& name, int count, int* value)
{
	glUniform1iv(GetUniformLocation(name), count, value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform4fv(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	//glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));

}

int Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformlocationCache.find(name) != m_UniformlocationCache.end())
	{
		return m_UniformlocationCache[name];
	}
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist" << std::endl;
	}
	m_UniformlocationCache[name] = location;
	return location;
}
