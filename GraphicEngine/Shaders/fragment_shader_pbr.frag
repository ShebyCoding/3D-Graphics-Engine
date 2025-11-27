#version 460 core

struct Material
{
	vec3 albedo;
	float metallic;
	float roughness;
	float ambientOcclusion;

	sampler2D albedoMap;
	sampler2D normalMap;
	sampler2D metallicMap;
	sampler2D roughnessMap;
	sampler2D ambientOcclusionMap;

	bool hasAlbedoMap;
	bool hasNormalMap;
	bool hasMetallicMap;
	bool hasRoughnessMap;
	bool hasAmbientOcclusionMap;
};

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texture_coord;

out vec4 fragment_color;

uniform Material material;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 camera_position;

const float PI = 3.14159265359;

// vertex_normal Distribution function (GGX)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// Geometry function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

// Geometry function (Smith)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

// Fresnel (Schlick)
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	// Sample textures
	vec3 albedo = material.albedo;
	if (material.hasAlbedoMap)
		albedo *= texture(material.albedoMap, vertex_texture_coord).rgb;

	float metallic = material.metallic;
	if (material.hasMetallicMap)
		metallic *= texture(material.metallicMap, vertex_texture_coord).r;

	float roughness = material.roughness;
	if (material.hasRoughnessMap)
		roughness *= texture(material.roughnessMap, vertex_texture_coord).r;

	float ambientOcclusion = material.ambientOcclusion;
	if (material.hasAmbientOcclusionMap)
		ambientOcclusion *= texture(material.ambientOcclusionMap, vertex_texture_coord).r;

	vec3 N = normalize(vertex_normal);
	vec3 V = normalize(camera_position - vertex_position);
	vec3 L = normalize(light_position - vertex_position);
	vec3 H = normalize(V + L);

	float distance = length(light_position - vertex_position);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light_color * attenuation;

	float NDF = DistributionGGX(N,H,roughness);
	float G = GeometrySmith(N,V,L,roughness);
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 F = fresnelSchlick(max(dot(H,V),0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N,V),0.0) * max(dot(N,L),0.0) + 0.001;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N,L),0.0);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 diffuse = kD * albedo / PI;

	vec3 result = (diffuse + specular) * radiance * NdotL * ambientOcclusion;

	fragment_color = vec4(result, 1.0);
}