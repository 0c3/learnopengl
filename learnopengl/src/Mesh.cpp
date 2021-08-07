#include "pch.h"
#include "Mesh.h"

#include "Shader.h"
#include "Texture.h"

#include <glad/glad.h>

Mesh::Mesh(vertex_t* _vertices, uint32_t _vertCount, uint32_t* _indices, uint32_t _idxCount, Texture* _textures, uint8_t _texCount)
{
	size_t vertSize = sizeof(vertex_t) * _vertCount;
	size_t idxSize = sizeof(uint32_t) * _idxCount;

	vertCount = _vertCount;
	vertices = (vertex_t*)malloc(vertSize);
	if (vertices)
		memcpy(vertices, _vertices, vertSize);

	idxCount = _idxCount;
	indices = (uint32_t*)malloc(idxSize);
	if (indices)
		memcpy(indices, _indices, idxSize);

	texCount = _texCount;
	textures = nullptr;
	if (_texCount > 0)
	{
		size_t texSize = sizeof(Texture) * _texCount;
		textures = (Texture*)malloc(texSize);
		if (textures)
		{
			memcpy(textures, _textures, texSize);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertSize, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize, &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, uv));
}

Mesh::Mesh(const char* path)
{
	FILE* fp = fopen(path, "rb");

	if (!fp)
	{
		char* str = (char*)malloc(11 + strlen(path));
		if (str)
		{
			strcpy(str, "res/model/");
			fp = fopen(strcat(str, path), "rb");
			free(str);
		}
		if (!fp)
			perror(path), exit(1);
	}

	// get line type specifier
	/*if (fscanf(fp, "%s", str))
	{
		// check if this line is a vertex position
		if (strcmp(str, "v") == 0)
		{
			fscanf(fp, "%f%f%f", &f1, &f2, &f3);
			printf("%f %f %f\n", f1, f2, f3);
		}
	}*/

	char line[512];

	for (fgets(line, 100, fp); !feof(fp); fgets(line, 100, fp))
	{
		if (line[0] == 'v' && line[1] == ' ')
			vertCount++;
		if (line[0] == 'f' && line[1] == ' ')
			idxCount += 3;
	}

	size_t vertSize = sizeof(vertex_t) * vertCount;
	vertices = (vertex_t*)malloc(vertSize);

	size_t idxSize = sizeof(vertex_t) * idxCount;
	indices = (uint32_t*)malloc(idxSize);

	int itVertex = 0; // vertex iterator
	int itIndex = 0; // index iterator
	rewind(fp);
	for (fgets(line, 100, fp); !feof(fp); fgets(line, 100, fp))
	{
		// if this line is vertex position
		if (line[0] == 'v' && line[1] == ' ')
		{
			glm::vec3 vertex;
			if (sscanf(line + 2, "%f%f%f", &vertex.x, &vertex.y, &vertex.z) == 3)
			{
				vertices[itVertex].position = vertex;
				itVertex++;
			}
		}
		// if is face (for indices)
		if (line[0] == 'f' && line[1] == ' ')
		{
			uint32_t idx[3];
			if (sscanf(line + 2, "%u/%*u/%*u %u/%*u/%*u %u/%*u/%*u", &idx[0], &idx[1], &idx[2]) == 3)
			{
				for (uint8_t i = 0; i < 3; i++)
				{
					indices[itIndex+i] = idx[i] - 1;
				}
				itIndex += 3;
			}
		}
	}

	fclose(fp);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, &vertices[0], GL_STATIC_DRAW);

	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize, &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, uv));
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	free(vertices);
	free(indices);
	free(textures);
}

void Mesh::Draw(Shader& shader)
{
	if (texCount > 0)
	{
		uint8_t numDiffuse = 0;
		uint8_t numSpecular = 0;
		for (uint8_t i = 0; i < texCount; i++)
		{
			char buf[28];
			uint8_t texType = textures[i].type;
			if (texType == TextureType::Diffuse)
				sprintf(buf, "material.texture_diffuse%hhu", numDiffuse++);
			if (texType == TextureType::Specular)
				sprintf(buf, "material.texture_specular%hhu", numSpecular++);
			shader.SetValue(buf, i);
			textures[i].Bind(i);
		}
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, vertCount);
}