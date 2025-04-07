#include "glpch.h"
#include "Texture2D.h"
#include "glad/glad.h"
#include "stb_image.h"
Texture2D::Texture2D(const std::string& path):
	m_RendererID(0),m_FilePath(path),m_LocalBuffer(nullptr),m_Width(0),m_Height(0), m_Channels(0)
{
	stbi_set_flip_vertically_on_load(true);
	//desired_channels：最后一个参数为期望通道数，用于指定加载后图像数据的 ​目标通道数，0保持原图通道数，1，3，4 强制将图像转换为对应通道数
	 m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
	//m_LocalBuffer = stbi_load("E:/block_solid.png", &m_Width, &m_Height, &m_Channels, 0);
	// std::cout << "图片路径" <<path<< std::endl;
	if (!m_LocalBuffer)
	//{
	//	// 打印前4个像素的RGB值（假设 channels=4）
	//	for (int i = 0; i < 4 * 4; i += 4)
	//	{
	//		printf("Pixel %d: (%d, %d, %d, %d)\n", i / 4, m_LocalBuffer[i], m_LocalBuffer[i + 1], m_LocalBuffer[i + 2], m_LocalBuffer[i + 3]);
	//	}
	//}
	//else
	{
		printf("Failed to load texture!\n");
	}

	GLenum format = GL_RGB;
	if (m_Channels == 4)
		format = GL_RGBA;
	else if (m_Channels == 1)
		format = GL_RED;

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cerr << "OpenGL Error after glTexImage2D: " << err << std::endl;
	}

	// 分配缓冲区
	//std::vector<GLubyte> readBackData(m_Width * m_Height * m_Channels);
	//
	//glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, readBackData.data());

	//// 对比数据
	//if (memcmp(m_LocalBuffer, readBackData.data(), m_Width * m_Height * m_Channels) == 0)
	//{
	//	std::cout << "数据一致，上传成功" << std::endl;
	//}
	//else
	//{
	//	std::cerr << "数据不一致，上传失败" << std::endl;
	//}

	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture2D::Bind(unsigned int slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 +slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
	this->m_Width = width;
	this->m_Height = height;
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, this->m_RendererID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

