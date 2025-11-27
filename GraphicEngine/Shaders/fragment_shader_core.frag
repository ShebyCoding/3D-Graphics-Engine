#version 460 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	sampler2D diffuse_sampler;
	sampler2D specular_sampler;

	bool hasDiffuseMap;
	bool hasSpecularMap;
};

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_color;
in vec2 vertex_texture_coord;

out vec4 fragment_color;

uniform Material material;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 camera_position;

void main()
{
	// --- Normalize everything ---
	vec3 N = normalize(vertex_normal);
	vec3 L = normalize(light_position - vertex_position);
	vec3 V = normalize(camera_position - vertex_position);
	vec3 H = normalize(L + V);// Blinn-Phong half vector (better & faster)

	// --- Texture color ---
	vec3 baseColor = vertex_color;
	if (material.hasDiffuseMap)
		baseColor = texture(material.diffuse_sampler, vertex_texture_coord).rgb;

	// --- Ambient ---
	vec3 ambient = material.ambient * baseColor;

	// --- Diffuse ---
	vec3 diffuse = material.diffuse * baseColor * max(dot(N, L), 0.0);

	// --- Specular ---
	vec3 specular = vec3(0.0);
	if (material.hasSpecularMap)
	{
		float specIntensity = texture(material.specular_sampler, vertex_texture_coord).r;
		specular = material.specular * specIntensity * pow(max(dot(N, H), 0.0), material.shininess);
	}

	// --- Attenuation ---
	float toLightDistance = length(light_position - vertex_position);
	float attenuation = 1.0 / (1.0 + 0.09 * toLightDistance + 0.032 * toLightDistance * toLightDistance);
	vec3 radiance = light_color * attenuation;

	// --- Final color ---
	vec3 result = (ambient + diffuse + specular) * radiance;
	result = pow(result, vec3(1.0/2.2));

	fragment_color = vec4(result, 1.0);
}