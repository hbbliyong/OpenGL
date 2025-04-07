#pragma once
#include <GLCore/Renderer/Texture2D.h>
#include <GLCore/Renderer/Shader.h>
#include "SpriteRenderer.h"

class PostProcessor
{
public:
	PostProcessor(std::shared_ptr<Shader> postProcessingShader, uint32_t width, uint32_t height);
	// Prepares the postprocessor's framebuffer operations before rendering the game
	void BeginRender();
	// Should be called after rendering the game, so it stores all the rendered data into a texture object
	void EndRender();
	// Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
	void Render(float time);
private:
	void initRenderData();
public:
	std::shared_ptr<Shader> m_PostProcessingShader;
	std::shared_ptr<Texture2D> m_Texture;
	uint32_t Width, Height;
	bool  Confuse = false;
	bool Chaos = false;
	bool Shake=false;
private:
	GLuint MSFBO, FBO; //MSFBO = Multisampled FBO.FBO is regular, used for blitting MS color - buffer to texture
	GLuint RBO;// RBO is used for multisampled color buffer
	GLuint VAO;
};