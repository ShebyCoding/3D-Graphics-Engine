#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texture_coord;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec3 vertex_color;
out vec2 vertex_texture_coord;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
	vertex_position = vec4(model_matrix * vec4(position, 1.0f)).xyz;
	vertex_normal = mat3(model_matrix) * normal;
	vertex_color = color;
	vertex_texture_coord = texture_coord;

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
}