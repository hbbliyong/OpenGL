#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader
{
public:
Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();

	GLuint GetRendererID() { return m_RendererID; }

	//static Shader* FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

	void Bind() const;
	void UnBind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4fv(const std::string& name, const glm::vec4& value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	

	void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	GLuint CompileShader(GLenum type, const std::string& source);
	//ª∫¥ÊUniform±‰¡ø
	int GetUniformLocation(const std::string& name) const;
private:
	GLuint m_RendererID;
	mutable	std::unordered_map<std::string, int> m_UniformlocationCache;
};

