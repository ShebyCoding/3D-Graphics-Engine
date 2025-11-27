#pragma once

#include <map>
#include <memory>
#include <string>

#include "shader.h"
#include "texture.h"

class ResourceManager
{
public:
	static std::map<std::string, std::shared_ptr<Shader>> Shaders;
	static std::map<std::string, std::shared_ptr<Texture>> Textures;

	static std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto s = std::make_shared<Shader>(vertexPath, fragmentPath);
		Shaders[name] = s;
		return s;
	}

	static std::shared_ptr<Texture> loadTexture(const std::string& name, const std::string& texturePath, GLenum textureType = GL_TEXTURE_2D)
	{
		auto t = std::make_shared<Texture>(texturePath, textureType);
		Textures[name] = t;
		return t;
	}

	static std::shared_ptr<Shader> getShader(const std::string& name)
	{
		return Shaders.count(name) ? Shaders[name] : nullptr;
	}

	static std::shared_ptr<Texture> getTexture(const std::string& name)
	{
		return Textures.count(name) ? Textures[name] : nullptr;
	}

	static void clear()
	{
		Shaders.clear();
		Textures.clear();
	}
};