#pragma once
#include <map>
#include <string>
#include <memory>
#include "GLCore/Renderer/Texture2D.h"
#include "GLCore/Renderer/Shader.h"

class ResourceManager
{
public:
	//Resource storage
	static std::map<std::string, std::shared_ptr<Shader>> Shaders;
	static std::map<std::string, std::shared_ptr<Texture2D>> Textures;

	static std::shared_ptr < Shader> GetShader(std::string& name);
	static std::shared_ptr < Shader> LoadShader(const std::string& vShaderFiles, const std::string& fShaderFiles, const std::string& gShaderFiles, std::string& name);
	static std::shared_ptr < Texture2D> LoadTexture(const std::string& file, bool alpha, std::string& name);
	static std::shared_ptr < Texture2D> GetTexture(std::string& name);

	static void Clear();

private:
	ResourceManager() = default;
};