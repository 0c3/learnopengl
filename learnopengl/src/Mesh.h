#pragma once

#include <glm/glm.hpp>

struct vertex_t
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Shader;

class Texture;

class Mesh
{
public:
	Mesh(vertex_t* _vertices, uint32_t _vertCount, uint32_t* _indices, uint32_t _idxCount, Texture* _textures, uint8_t _texCount);
	Mesh(const char* path); // load file
	~Mesh();

	void Draw(Shader& shader);

public:
	vertex_t* vertices;
	uint32_t vertCount;

	uint32_t* indices;
	uint32_t idxCount;

	Texture* textures;
	uint8_t texCount;

	uint32_t VAO, VBO, EBO;
};