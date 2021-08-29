#pragma once

#include <glm/glm.hpp>

struct vertex_t
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

#include "Shader.h"

class Texture;

class Mesh
{
public:
	Mesh(vertex_t* _vertices, uint32_t _vertCount, uint32_t* _indices, uint32_t _idxCount, Texture* _textures, uint8_t _texCount, Shader _shader = Shader());
	Mesh(const char* path, Shader _shader = Shader()); // load file
	Mesh(Shader* _shader, Texture* _textures = nullptr, uint8_t _texCount = 0); // default mesh
	~Mesh();

	void Draw();

public:
	vertex_t* vertices;
	uint32_t vertCount;

	uint32_t* indices;
	uint32_t idxCount;

	Texture* textures;
	uint8_t texCount;

	uint32_t VAO, VBO, EBO;

	bool unindexedDrawing;

	Shader shader;
};