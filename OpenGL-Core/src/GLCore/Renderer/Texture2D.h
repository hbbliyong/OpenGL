#pragma once
#include <string>
#include "glad/glad.h"
class Texture2D
{
public:
	Texture2D()=default;
	Texture2D(const std::string& path);
	~Texture2D();

	void Bind(unsigned int slot = 0)const;
	void UnBind() const;

	inline int GetWidth()const { return m_Width; }
	inline int GetHeight()const { return m_Height; }
	inline unsigned int ID()const { return m_RendererID; }

	void Generate(GLuint width, GLuint height, unsigned char* data);
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_Channels;
};