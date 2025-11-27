#pragma once

#include "shader.h"
#include "texture.h"

class PhongMaterial
{
private:
	// Material parameters
	glm::vec3 _ambient = glm::vec3(0.1f);
	glm::vec3 _diffuse = glm::vec3(1.0f);
	glm::vec3 _specular = glm::vec3(1.0f);
	float _shininess = 32.0f;

	// Optional textures
	const Texture* _diffuseMap = nullptr;
	const Texture* _specularMap = nullptr;

public:
	PhongMaterial() = default;

	PhongMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const Texture* diffuseMap = nullptr, const Texture* specularMap = nullptr)
		: _ambient(ambient), _diffuse(diffuse), _specular(specular), _shininess(shininess), _diffuseMap(diffuseMap), _specularMap(specularMap) {
	}

	~PhongMaterial() = default;

	// Set textures
	void setDiffuseMap(const Texture* tex) { _diffuseMap = tex; }
	void setSpecularMap(const Texture* tex) { _specularMap = tex; }

	// Send everything to shader
	void apply(Shader& shader) const
	{
		shader.use();

		shader.set("material.ambient", _ambient);
		shader.set("material.diffuse", _diffuse);
		shader.set("material.specular", _specular);
		shader.set("material.shininess", _shininess);

		shader.set("material.hasDiffuseMap", _diffuseMap != nullptr);
		shader.set("material.hasSpecularMap", _specularMap != nullptr);

		int textureUnit = 0;

		if (_diffuseMap)
		{
			shader.set("material.diffuse_sampler", textureUnit);
			_diffuseMap->bind(textureUnit);
			textureUnit++;
		}

		if (_specularMap)
		{
			shader.set("material.specular_sampler", textureUnit);
			_specularMap->bind(textureUnit);
			textureUnit++;
		}
	}
};

class PBRMaterial
{
private:
	// Material parameters
	glm::vec3 _albedo = glm::vec3(1.0f);
	float _metallic = 0.0f;
	float _roughness = 0.5f;
	float _ambientOcclusion = 1.0f;

	// Optional textures
	const Texture* _albedoMap = nullptr;
	const Texture* _normalMap = nullptr;
	const Texture* _metallicMap = nullptr;
	const Texture* _roughnessMap = nullptr;
	const Texture* _ambientOcclusionMap = nullptr;

public:
	PBRMaterial() = default;

	PBRMaterial(const glm::vec3& albedo, float metallic = 0.0f, float roughness = 0.5f, float ambientOcclusion = 1.0f, const Texture* albedoMap = nullptr, const Texture* normalMap = nullptr, const Texture* metallicMap = nullptr, const Texture* roughnessMap = nullptr, const Texture* ambientOcclusionMap = nullptr)
		: _albedo(albedo), _metallic(metallic), _roughness(roughness), _ambientOcclusion(ambientOcclusion), _albedoMap(albedoMap), _normalMap(normalMap), _metallicMap(metallicMap), _roughnessMap(roughnessMap), _ambientOcclusionMap(ambientOcclusionMap) {
	}

	~PBRMaterial() = default;

	// Setters
	void setAlbedoMap(const Texture* tex) { _albedoMap = tex; }
	void setNormalMap(const Texture* tex) { _normalMap = tex; }
	void setMetallicMap(const Texture* tex) { _metallicMap = tex; }
	void setRoughnessMap(const Texture* tex) { _roughnessMap = tex; }
	void setAOMap(const Texture* tex) { _ambientOcclusionMap = tex; }

	// Bind PBR material parameters to shader
	void apply(Shader& shader) const
	{
		shader.use();

		// Base values
		shader.set("material.albedo", _albedo);
		shader.set("material.metallic", _metallic);
		shader.set("material.roughness", _roughness);
		shader.set("material.ambientOcclusion", _ambientOcclusion);

		shader.set("material.hasAlbedoMap", _albedoMap != nullptr);
		shader.set("material.hasNormalMap", _normalMap != nullptr);
		shader.set("material.hasMetallicMap", _metallicMap != nullptr);
		shader.set("material.hasRoughnessMap", _roughnessMap != nullptr);
		shader.set("material.hasAmbientOcclusionMap", _ambientOcclusionMap != nullptr);

		int textureUnit = 0;

		if (_albedoMap)
		{
			shader.set("material.albedoMap", textureUnit);
			_albedoMap->bind(textureUnit++);
		}

		if (_normalMap)
		{
			shader.set("material.normalMap", textureUnit);
			_normalMap->bind(textureUnit++);
		}

		if (_metallicMap)
		{
			shader.set("material.metallicMap", textureUnit);
			_metallicMap->bind(textureUnit++);
		}

		if (_roughnessMap)
		{
			shader.set("material.roughnessMap", textureUnit);
			_roughnessMap->bind(textureUnit++);
		}

		if (_ambientOcclusionMap)
		{
			shader.set("material.ambientOcclusionMap", textureUnit);
			_ambientOcclusionMap->bind(textureUnit++);
		}
	}
};