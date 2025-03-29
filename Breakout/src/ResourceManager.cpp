#include "ResourceManager.h"

std::map<std::string, std::shared_ptr < Shader> > ResourceManager::Shaders;
std::map<std::string, std::shared_ptr< Texture2D>> ResourceManager::Textures;

std::shared_ptr <Shader> ResourceManager::LoadShader(const std::string& vShaderFiles, const std::string& fShaderFiles, const std::string& gShaderFiles, std::string& name)
{
	auto it = Shaders.find(name);
	if (it == Shaders.end())
	{
		it = Shaders.emplace(name, std::make_shared< Shader>(vShaderFiles, fShaderFiles)).first;
	}
	return it->second;
}

std::shared_ptr < Shader> ResourceManager::GetShader(std::string& name)
{
	return	Shaders[name];
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& file, bool alpha, std::string& name)
{
	auto it = Textures.find(name);
	if (it == Textures.end())
	{
		it = Textures.emplace(name, std::make_shared<Texture2D>(file)).first;
	}

	return it->second;
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(std::string& name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	Shaders.clear();
	Textures.clear();
}

