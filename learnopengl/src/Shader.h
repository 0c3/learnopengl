#pragma once

#include <stdint.h>
#include <glm/fwd.hpp>
#include <parallel_hashmap/phmap.h>

namespace shader_internal
{
	struct uniform_info_t
	{
		int32_t location;
		int32_t count;
	};
}

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	bool Reload(const char* vertexPath, const char* fragmentPath);

	void Use();

	void SetValue(const char* name, bool value);			  // bool
	void SetValue(const char* name, int32_t value);			  // int32_t
	void SetValue(const char* name, uint32_t value);		  // uint32_t
	void SetValue(const char* name, float value);			  // float

	// vec2
	void SetValue(const char* name, const glm::vec2& value);
	void SetValue(const char* name, float x, float y);
	// vec3
	void SetValue(const char* name, const glm::vec3& value);
	void SetValue(const char* name, float x, float y, float z);
	// vec4
	void SetValue(const char* name, const glm::vec4& value);
	void SetValue(const char* name, float x, float y, float z, float w);


	// matrices
	void SetValue(const char* name, const glm::mat2& value);  // mat2
	void SetValue(const char* name, const glm::mat3& value);  // mat3
	void SetValue(const char* name, const glm::mat4& value);  // mat4

	// arrays
	void SetArray(const char* name, float value);

public:
	uint32_t ID;
	phmap::flat_hash_map<uint32_t, shader_internal::uniform_info_t> uniforms;
};