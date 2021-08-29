#include "pch.h"
#include "Object.h"

Object::Object()
{
	transform = {
		glm::vec3(0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f)
	};
	mesh = nullptr;
}

Object::Object(Mesh* _mesh)
{
	transform = {
		glm::vec3(0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f)
	};
	mesh = _mesh;
}

void Object::AddMesh(Mesh* _mesh)
{
	mesh = _mesh;
}