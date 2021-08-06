#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

class FlyCam
{
public:
	FlyCam(glm::vec3 _pos, glm::vec2 _ang, float _fov = 45.0f, float _nearPlane = 0.1f, float _farPlane = 100.0f);

	void processInput(int32_t key, float deltaTime);
	void mouseCallback(float xOffset, float yOffset);
	void scrollCallback(float yOffset);

	glm::mat4 vpMatrix();

public:
	float fov;
	float nearPlane, farPlane;

	glm::vec3 pos;
	glm::vec2 ang;
	glm::vec3 dir; // points in reverse direction

	glm::vec3 right, up;

public:
	void updateVectors(); // utility function
};