#include "pch.h"

#include <glm/gtc/matrix_transform.hpp>

#include "ObjectWorld.h"
#include "Object.h"
#include "Mesh.h"

ObjectWorld::ObjectWorld()
{

}

Object& ObjectWorld::CreateObject()
{
	Object object;
	objects.push_back(object);
	return objects.back();
}

void ObjectWorld::Update(float dt)
{
	for (const Object& obj : objects)
	{
		if (obj.mesh)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, obj.transform.pos);
			//model = glm::rotate(model, );
			model = glm::scale(model, obj.transform.scale);

			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			obj.mesh->shader.SetValue("model", model);
			//shader.SetValue("time", (float)glfwGetTime());
			obj.mesh->shader.SetValue("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
			obj.mesh->Draw();
		}
	}
}