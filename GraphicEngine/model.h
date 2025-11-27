#pragma once

#include "mesh.h"
#include "tiny_obj_loader.h"

class Model
{
private:
	std::vector<Mesh*> _meshes;

public:
	Model() = default;

	explicit Model(const std::string& filepath)
	{
		loadModelData(filepath);
	}

	~Model()
	{
		for (auto* m : _meshes)
			delete m;
	}

	void setPosition(const glm::vec3& position) { for (auto m : _meshes) m->setPosition(position); }
	void setRotation(const glm::vec3& rotation) { for (auto m : _meshes) m->setRotation(rotation); }
	void setScale(const glm::vec3& scale) { for (auto m : _meshes) m->setScale(scale); }
	void move(const glm::vec3& deltaPosition) { for (auto m : _meshes) m->move(deltaPosition); }
	void rotate(const glm::vec3& deltaRotation) { for (auto m : _meshes) m->rotate(deltaRotation); }
	void scale(const glm::vec3& deltaScale) { for (auto m : _meshes) m->scale(deltaScale); }

	size_t getTotalVertexCount() const
	{
		size_t total = 0;
		for (auto m : _meshes)
			total += m->getVertexCount();
		return total;
	}

	void render(const Shader& shader)
	{
		for (auto* mesh : _meshes)
			mesh->render(shader);
	}

private:
	void loadModelData(const std::string& filepath)
	{
		tinyobj::attrib_t vertexAttributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		// Try to load *.Obj and pass the data as Vertex, Meshes (shapes), Materials and messages
		bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warn, &err, filepath.c_str(), nullptr, true);

		// Load check and output any warnings/errors
		if (!warn.empty())
			std::cout << "TinyObj WARN: " << warn << "\n";

		if (!err.empty())
			std::cerr << "TinyObj ERR: " << err << "\n";

		if (!success)
		{
			std::cerr << "Failed to load OBJ file: " << filepath << "\n";
			return;
		}

		std::cout << "Loaded OBJ: " << filepath << "\n";

		bool hasNormals = !vertexAttributes.normals.empty();
		bool hasTexcoords = !vertexAttributes.texcoords.empty();

		// Process shapes (each shape is mesh)
		for (const auto& shape : shapes)
		{
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;

			vertices.reserve(shape.mesh.indices.size());
			indices.reserve(shape.mesh.indices.size());

			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				// POSITION
				vertex.position =
				{
					vertexAttributes.vertices[3 * index.vertex_index + 0],
					vertexAttributes.vertices[3 * index.vertex_index + 1],
					vertexAttributes.vertices[3 * index.vertex_index + 2]
				};

				// NORMAL
				if (hasNormals && index.normal_index >= 0)
				{
					vertex.normal =
					{
						vertexAttributes.normals[3 * index.normal_index + 0],
						vertexAttributes.normals[3 * index.normal_index + 1],
						vertexAttributes.normals[3 * index.normal_index + 2]
					};
				}
				else
				{
					// fallback
					vertex.normal = { 0, 0, 1 };
				}

				// TEXCOORD
				if (hasTexcoords && index.texcoord_index >= 0)
				{
					vertex.textureCoord =
					{
						vertexAttributes.texcoords[2 * index.texcoord_index + 0],
						1.0f - vertexAttributes.texcoords[2 * index.texcoord_index + 1]
					};
				}
				else
				{
					// fallback
					vertex.textureCoord = { 0, 0 };
				}

				// COLOR default (OBJ does not store vertex colors)
				vertex.color = { 1, 1, 1 };

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}

			// Create mesh
			Mesh* mesh = new Mesh(vertices, indices);
			_meshes.push_back(mesh);
		}
	}
};