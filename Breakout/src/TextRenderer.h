#pragma once

#include "GLCore/Renderer/Texture2D.h"
#include "GLCore/Renderer/Shader.h"
#include "GLCore/Core/Core.h"
#include <map>

struct Character
{
	GLuint TextureID;//ID handle of the glyph texture
	glm::ivec2 Size;
	glm::ivec2 Bearing;// Offset from baseline to left/top of glyph
	GLuint Advance;// Horizontal offset to advance to next glyph
};

// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
	TextRenderer(GLuint width, GLuint height);
	void Load(const std::string& font, GLuint fontSize);
	void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
public:
	std::map<GLchar, Character> Characters;
	Ref<Shader> TextShader;
private:
	GLuint m_VAO;
	GLuint m_VBO;
};