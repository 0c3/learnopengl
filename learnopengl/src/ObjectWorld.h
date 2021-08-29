#pragma once

#include <vector>

#include "Object.h"

class ObjectWorld
{
public:
	ObjectWorld();

	Object& CreateObject();

	void Update(float dt);

public:
	std::vector<Object> objects;
};