#pragma once

#include <fwd.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 textureCoord;
};

class Primitive
{
protected:
	std::vector<Vertex> _vertices;
	std::vector<unsigned> _indices;
	glm::vec3 _defaultColor = glm::vec3(1.0f);

public:
	Primitive() = default;
	virtual ~Primitive() = default;

	const std::vector<Vertex>& getVertices() const { return _vertices; }
	const std::vector<unsigned>& getIndices() const { return _indices; }
};

class Plane : public Primitive
{
public:
	Plane(float width = 1.0f, float height = 1.0f)
		: Primitive()
	{
		float w = width * 0.5f;
		float h = height * 0.5f;

		// 4 vertices
		_vertices =
		{
			{{-w, 0,-h}, { 0, 1, 0}, _defaultColor, {0, 1}},
			{{-w, 0, h}, { 0, 1, 0}, _defaultColor, {0, 0}},
			{{ w, 0, h}, { 0, 1, 0}, _defaultColor, {1, 0}},
			{{ w, 0,-h}, { 0, 1, 0}, _defaultColor, {1, 1}}
		};

		// 6 indices (2 triangles × 3 vertices)
		_indices =
		{
			0, 1, 2,
			0, 2, 3
		};
	}
};

class Cube : public Primitive
{
public:
	Cube(float size = 1.0f)
		: Primitive()
	{
		float s = size * 0.5f;

		// 24 vertices (4 per face)
		_vertices =
		{
			// Front
			{{-s, s, s}, { 0, 0, 1}, _defaultColor, {0, 1}},
			{{-s,-s, s}, { 0, 0, 1}, _defaultColor, {0, 0}},
			{{ s,-s, s}, { 0, 0, 1}, _defaultColor, {1, 0}},
			{{ s, s, s}, { 0, 0, 1}, _defaultColor, {1, 1}},

			// Back
			{{ s, s,-s}, { 0, 0,-1}, _defaultColor, {0, 1}},
			{{ s,-s,-s}, { 0, 0,-1}, _defaultColor, {0, 0}},
			{{-s,-s,-s}, { 0, 0,-1}, _defaultColor, {1, 0}},
			{{-s, s,-s}, { 0, 0,-1}, _defaultColor, {1, 1}},

			// Left
			{{-s, s,-s}, {-1, 0, 0}, _defaultColor, {0, 1}},
			{{-s,-s,-s}, {-1, 0, 0}, _defaultColor, {0, 0}},
			{{-s,-s, s}, {-1, 0, 0}, _defaultColor, {1, 0}},
			{{-s, s, s}, {-1, 0, 0}, _defaultColor, {1, 1}},

			// Right
			{{ s, s, s}, { 1, 0, 0}, _defaultColor, {0, 1}},
			{{ s,-s, s}, { 1, 0, 0}, _defaultColor, {0, 0}},
			{{ s,-s,-s}, { 1, 0, 0}, _defaultColor, {1, 0}},
			{{ s, s,-s}, { 1, 0, 0}, _defaultColor, {1, 1}},

			// Top
			{{-s, s,-s}, { 0, 1, 0}, _defaultColor, {0, 1}},
			{{-s, s, s}, { 0, 1, 0}, _defaultColor, {0, 0}},
			{{ s, s, s}, { 0, 1, 0}, _defaultColor, {1, 0}},
			{{ s, s,-s}, { 0, 1, 0}, _defaultColor, {1, 1}},

			// Bottom
			{{-s,-s, s}, { 0,-1, 0}, _defaultColor, {0, 1}},
			{{-s,-s,-s}, { 0,-1, 0}, _defaultColor, {0, 0}},
			{{ s,-s,-s}, { 0,-1, 0}, _defaultColor, {1, 0}},
			{{ s,-s, s}, { 0,-1, 0}, _defaultColor, {1, 1}},
		};

		// 36 indices (6 faces × 2 triangles × 3 vertices)
		_indices =
		{
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
	}
};

class Sphere : public Primitive
{
public:
	Sphere(float radius = 1.0f, unsigned horizontalLines = 16, unsigned verticalLines = 32)
		: Primitive()
	{
		if (horizontalLines < 2 || verticalLines < 2)
			return;

		for (unsigned i = 0; i <= horizontalLines; ++i)
		{
			float texY = float(i) / horizontalLines;
			float phi = (1.0f - texY) * glm::pi<float>();

			for (unsigned j = 0; j <= verticalLines; ++j)
			{
				float texX = float(j) / verticalLines;
				float theta = texX * glm::two_pi<float>();

				glm::vec3 normal =
				{
					std::sin(phi) * std::cos(theta),
					std::cos(phi),
					std::sin(phi) * std::sin(theta)
				};

				_vertices.push_back({ normal * radius, normal, _defaultColor, { texX, texY } });
			}
		}

		// Count of indices = horizontalLines * verticalLines × 2 triangles × 3 vertices)
		for (unsigned i = 0; i < horizontalLines; ++i)
		{
			for (unsigned j = 0; j < verticalLines; ++j)
			{
				unsigned a = i * (verticalLines + 1) + j;
				unsigned b = a + verticalLines + 1;

				_indices.push_back(a);
				_indices.push_back(b);
				_indices.push_back(b + 1);

				_indices.push_back(a);
				_indices.push_back(b + 1);
				_indices.push_back(a + 1);
			}
		}
	}
};

class Torus : public Primitive
{
public:
	Torus(float outerRadius = 1.0f, float innerRadius = 0.3f, unsigned horizontalLines = 16, unsigned verticalLines = 16)
		: Primitive()
	{
		if (horizontalLines < 2 || verticalLines < 2)
			return;

		for (unsigned i = 0; i <= horizontalLines; ++i)
		{
			float texY = float(i) / horizontalLines;
			float phi = (1.0f - texY) * glm::two_pi<float>();

			for (unsigned j = 0; j <= verticalLines; ++j)
			{
				float texX = float(j) / verticalLines;
				float theta = texX * glm::two_pi<float>();

				float x = (outerRadius + innerRadius * glm::cos(theta)) * glm::cos(phi);
				float y = innerRadius * glm::sin(theta);
				float z = (outerRadius + innerRadius * glm::cos(theta)) * glm::sin(phi);

				glm::vec3 pos(x, y, z);

				glm::vec3 normal =
				{
					glm::cos(theta) * glm::cos(phi),
					glm::sin(theta),
					glm::cos(theta) * glm::sin(phi)
				};

				_vertices.push_back({ pos, normal, _defaultColor, { texX, texY } });
			}
		}

		// Count of indices = horizontalLines * verticalLines × 2 triangles × 3 vertices)
		for (unsigned i = 0; i < horizontalLines; ++i)
		{
			for (unsigned j = 0; j < verticalLines; ++j)
			{
				unsigned a = i * (verticalLines + 1) + j;
				unsigned b = a + verticalLines + 1;

				_indices.push_back(a);
				_indices.push_back(b);
				_indices.push_back(b + 1);

				_indices.push_back(a);
				_indices.push_back(b + 1);
				_indices.push_back(a + 1);
			}
		}
	}
};