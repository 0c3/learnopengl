#pragma once

#include <glm/glm.hpp>

struct vertex_t
{
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec2 uv;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Draw();

public:
	vertex_t* vertices;
	uint16_t vertCount;
};