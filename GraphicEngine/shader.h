#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <glad.h>
#include <fwd.hpp>
#include <gtc/type_ptr.hpp>

class Shader
{
private:
	GLuint _id = 0;
	mutable std::unordered_map<std::string, GLint> _uniformCache;

public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "")
	{
		GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
		GLuint geometryShader = 0;
		GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

		// Only load geometry shader if a valid path was provided
		if (!geometryPath.empty())
			geometryShader = loadShader(geometryPath, GL_GEOMETRY_SHADER);

		// Link the shaders into a program
		_id = linkProgram(vertexShader, geometryShader, fragmentShader);

		// Cleanup shader objects (but only if they were created)
		if (vertexShader)
			glDeleteShader(vertexShader);

		if (geometryShader)
			glDeleteShader(geometryShader);

		if (fragmentShader)
			glDeleteShader(fragmentShader);

		// Validate
		if (!_id)
			std::cerr << "ERROR::SHADER::PROGRAM_CREATION_FAILED" << "\n";
	}

	~Shader()
	{
		if (_id) glDeleteProgram(_id);
	}

	void use() const
	{
		if (_id) glUseProgram(_id);
	}

	template<typename T>
	void set(const std::string& name, const T& value) const
	{
		GLint loc = getUniformLocation(name);

		if constexpr (std::is_same_v<T, int>)
			glUniform1i(loc, value);

		else if constexpr (std::is_same_v<T, bool>)
			glUniform1i(loc, (int)value);

		else if constexpr (std::is_same_v<T, float>)
			glUniform1f(loc, value);

		else if constexpr (std::is_same_v<T, glm::vec2>)
			glUniform2fv(loc, 1, glm::value_ptr(value));

		else if constexpr (std::is_same_v<T, glm::vec3>)
			glUniform3fv(loc, 1, glm::value_ptr(value));

		else if constexpr (std::is_same_v<T, glm::vec4>)
			glUniform4fv(loc, 1, glm::value_ptr(value));

		else if constexpr (std::is_same_v<T, glm::mat3>)
			glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));

		else if constexpr (std::is_same_v<T, glm::mat4>)
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));

		else
			std::cerr << "ERROR::SHADER::SET_UNIFORM::UNSUPPORTED_TYPE for uniform '" << name << "'\n";
	}

private:
	GLuint loadShader(const std::string& shaderPath, GLenum shaderType) const
	{
		// Read shader file
		std::string fileContent = readFile(shaderPath);
		if (fileContent.empty())
		{
			std::cerr << "ERROR::SHADER::FILE_NOT_FOUND - " << shaderPath << "\n";
			return 0;
		}

		const GLchar* shaderCode = fileContent.c_str();

		// Create shader and compile it
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);

		// Check compilation status
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		// If compilation failed, print log and return 0
		if (!success)
		{
			GLint logLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());

			std::cerr << "ERROR::SHADER::COMPILATION_FAILED - " << shaderPath << "\n" << infoLog.data() << "\n";

			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	GLuint linkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader) const
	{
		// Validate shaders
		if (vertexShader == 0 || fragmentShader == 0)
		{
			std::cerr << "ERROR::SHADER::LINK_PROGRAM::INVALID_SHADER" << "\n";
			return 0;
		}

		// Create program, then attach and link shaders to it
		GLuint program = glCreateProgram();

		glAttachShader(program, vertexShader);
		if (geometryShader != 0)
			glAttachShader(program, geometryShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		// Check for linking errors
		GLint success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		// If linking failed, print log and return 0
		if (!success)
		{
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetProgramInfoLog(program, logLength, NULL, infoLog.data());

			std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED" << "\n" << infoLog.data() << "\n";

			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	GLint getUniformLocation(const std::string& name) const
	{
		auto uniform = _uniformCache.find(name);
		if (uniform != _uniformCache.end())
			return uniform->second;

		std::cout << "SHADER::GET_UNIFORM_LOCATION::CACHING - " << name << "\n";

		GLint location = glGetUniformLocation(_id, name.c_str());
		_uniformCache[name] = location;
		return location;
	}

	std::string readFile(const std::string& filePath) const
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			std::cerr << "ERROR::SHADER::READ_FILE_FAILED - " << filePath << "\n";
			return "";
		}

		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}
};