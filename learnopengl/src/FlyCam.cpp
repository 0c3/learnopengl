#include "pch.h"
#include "FlyCam.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

FlyCam::FlyCam(glm::vec3 _pos, glm::vec2 _ang, float _fov, float _nearPlane, float _farPlane)
	: pos(_pos), ang(_ang), fov(_fov), nearPlane(_nearPlane), farPlane(_farPlane)
{
    updateVectors();
}

void FlyCam::processInput(int32_t key, float deltaTime)
{
    float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (key == GLFW_KEY_W)
        pos += cameraSpeed * dir;
    if (key == GLFW_KEY_S)
        pos -= cameraSpeed * dir;
    if (key == GLFW_KEY_A)
        pos -= right * cameraSpeed;
    if (key == GLFW_KEY_D)
        pos += right * cameraSpeed;
    if (key == GLFW_KEY_SPACE)
        pos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_C)
        pos -= glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;

}

void FlyCam::mouseCallback(float xOffset, float yOffset)
{
    ang.x += xOffset;
    ang.y += yOffset;

    if (ang.y > 89.0f)
        ang.y = 89.0f;
    if (ang.y < -89.0f)
        ang.y = -89.0f;

    updateVectors();
}

void FlyCam::scrollCallback(float yOffset)
{
    static float maxFov = fov;
    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > maxFov)
        fov = maxFov;
}

glm::mat4 FlyCam::vpMatrix()
{
    return glm::perspective(glm::radians(fov), 800.0f / 600.0f, nearPlane, farPlane) * glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
}

void FlyCam::updateVectors()
{
    glm::vec3 direction(
        cos(glm::radians(ang.x)) * cos(glm::radians(ang.y)),
        sin(glm::radians(ang.y)),
        sin(glm::radians(ang.x)) * cos(glm::radians(ang.y)));

    dir = glm::normalize(direction);

    right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, dir));
}