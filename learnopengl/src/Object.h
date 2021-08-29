#pragma once

#include <glm/glm.hpp>

class Mesh;

struct transform_t
{
	glm::vec3 pos;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class Object
{
public:
	Object();
	Object(Mesh* _mesh);

	void AddMesh(Mesh* _mesh);

public:
	transform_t transform;
	Mesh* mesh;
};