#pragma once

#include "primitives.h"
#include "shader.h"

class Mesh
{
private:
	GLuint _vao = 0;
	GLuint _vbo = 0;
	GLuint _ebo = 0;

	size_t _vertexCount = 0;
	size_t _indexCount = 0;

	glm::vec3 _position = { 0, 0, 0 };
	glm::vec3 _rotation = { 0, 0, 0 };
	glm::vec3 _scale = { 1, 1, 1 };

	glm::mat4 _modelMatrix = glm::mat4(1.0f);

public:
	// Allow moving
	Mesh(Mesh&& other) noexcept
	{
		*this = std::move(other);
	}

	// By Mesh reference passing we want to prevent copying, this is so called "double deletion protection"
	Mesh(const Mesh&) = delete;

	// Constructor for Primitive parameter
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
	{
		initBuffers(vertices, indices);
		updateModelMatrix();
	}

	// Constructor for Primitive parameter
	Mesh(const Primitive& primitive)
	{
		initBuffers(primitive.getVertices(), primitive.getIndices());
		updateModelMatrix();
	}

	// Destructor for buffers
	~Mesh()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
	}

	// Operator overloads
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& other) noexcept
	{
		std::swap(_vao, other._vao);
		std::swap(_vbo, other._vbo);
		std::swap(_ebo, other._ebo);
		std::swap(_vertexCount, other._vertexCount);
		std::swap(_indexCount, other._indexCount);

		return *this;
	}

	// Functions to set and modify transform
	void setPosition(const glm::vec3& position) { _position = position; }
	void setRotation(const glm::vec3& rotation) { _rotation = rotation; }
	void setScale(const glm::vec3& scale) { _scale = scale; }
	void move(const glm::vec3& deltaPosition) { _position += deltaPosition; }
	void rotate(const glm::vec3& deltaRotation) { _rotation += deltaRotation; }
	void scale(const glm::vec3& deltaScale) { _scale += deltaScale; }

	// Get count of vertices and indices mesh
	size_t getVertexCount() const { return _vertexCount; }
	size_t getIndexCount() const { return _indexCount; }

	// Render function
	void render(const Shader& shader)
	{
		updateModelMatrix();

		shader.use();
		shader.set("model_matrix", _modelMatrix);

		glBindVertexArray(_vao);

		if (_indexCount > 0)
			glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, NULL);
		else
			glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
	}

private:
	void initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
	{
		_vertexCount = vertices.size();
		_indexCount = indices.size();

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _vertexCount * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		// Normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Color
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		// Texture Coord
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoord));

		glBindVertexArray(0);
	}

	void updateModelMatrix()
	{
		_modelMatrix = glm::mat4(1.0f);
		_modelMatrix = glm::translate(_modelMatrix, _position);
		_modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.x), { 1,0,0 });
		_modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.y), { 0,1,0 });
		_modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.z), { 0,0,1 });
		_modelMatrix = glm::scale(_modelMatrix, _scale);
	}
};