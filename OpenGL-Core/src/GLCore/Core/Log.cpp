#include "glpch.h"
#include "Log.h"
#include "glad/glad.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace GLCore {

	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt("GLCORE");
		s_Logger->set_level(spdlog::level::trace);
	}

}
void GLClearError()
{
	/* 循环获取错误(即清除) */
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): "
			<< function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}