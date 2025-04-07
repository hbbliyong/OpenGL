#include "PostProcessor.h"

PostProcessor::PostProcessor(std::shared_ptr<Shader> postProcessingShader, uint32_t width, uint32_t height) :
	m_PostProcessingShader(postProcessingShader), Width(width), Height(height)
{
	glGenFramebuffers(1, &MSFBO);
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);
	// Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, width, height);//Allocate storage for render buffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO);// Attach MS render buffer object to framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR:Failed to initialize MSFBO" << std::endl;
	}
	// Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	m_Texture = std::make_shared<Texture2D>();
	m_Texture->Generate(width, height, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->ID(), 0); //Attach texture to framebuffer as its color attachment
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Initialize render data and uniforms
	initRenderData();
	postProcessingShader->Bind();
	postProcessingShader->SetUniform1i("scene", 0);
	float offset = 1.0f / 300.0f;
	float offsets[9][2] = {
	  { -offset,  offset  },  // top-left
	  {  0.0f,    offset  },  // top-center
	  {  offset,  offset  },  // top-right
	  { -offset,  0.0f    },  // center-left
	  {  0.0f,    0.0f    },  // center-center
	  {  offset,  0.0f    },  // center - right
	  { -offset, -offset  },  // bottom-left
	  {  0.0f,   -offset  },  // bottom-center
	  {  offset, -offset  }   // bottom-right    
	};
	postProcessingShader->SetUniform2fv("offsets", 9, &offsets[0][0]);
	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	postProcessingShader->SetUniform1iv("edge_kernel", 9, edge_kernel);
	GLfloat blur_kernel[9] = {
	  1.0 / 16, 2.0 / 16, 1.0 / 16,
	  2.0 / 16, 4.0 / 16, 2.0 / 16,
	  1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	postProcessingShader->SetUniform1fv("blur_kernel", 9, blur_kernel);

}

void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
	// Now resolve multisampled color-buffer into intermediate FBO to store to texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);// Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::Render(float time)
{
	m_PostProcessingShader->Bind();
	m_PostProcessingShader->SetUniform1f("time", time);
	m_PostProcessingShader->SetUniform1i("confuse", Confuse);
	m_PostProcessingShader->SetUniform1i("chaos", Chaos);
	m_PostProcessingShader->SetUniform1i("shake", Shake);

	m_Texture->Bind();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
