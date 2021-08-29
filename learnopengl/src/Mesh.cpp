#include "pch.h"
#include "Mesh.h"

#include "Shader.h"
#include "Texture.h"

#include <glad/glad.h>

namespace GLDrawMode
{
	enum GLDrawMode {
		Indexed = 0,
		Arrays = 1
	};
}

Mesh::Mesh(vertex_t* _vertices, uint32_t _vertCount, uint32_t* _indices, uint32_t _idxCount, Texture* _textures, uint8_t _texCount, Shader _shader)
{
	shader = _shader;
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

Mesh::Mesh(const char* path, Shader _shader)
{
	shader = _shader;
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
	uint32_t normCount = 0;
	for (fgets(line, 100, fp); !feof(fp); fgets(line, 100, fp))
	{
		if (line[0] == 'v')
		{
			if (line[1] == ' ')
				vertCount++;
			if (line[1] == 'n')
				normCount++;
		}
		if (line[0] == 'f' && line[1] == ' ')
		{
			uint32_t x;
			static int8_t elementsRead = sscanf(line + 2, "%u/%*u/%u %u/%*u/%u %u/%*u/%u %u/%*u/%u", &x, &x, &x, &x, &x, &x, &x, &x);		
			idxCount += elementsRead / 2;
		}	
	}

	size_t vertSize = sizeof(vertex_t) * vertCount;
	vertices = (vertex_t*)malloc(vertSize);

	size_t normSize = sizeof(glm::vec3) * normCount;
	glm::vec3* normals = (glm::vec3*)malloc(normSize);

	size_t idxSize = sizeof(vertex_t) * idxCount;
	indices = (uint32_t*)malloc(idxSize);

	uint32_t itVertex = 0; // vertex iterator
	uint32_t itIndex = 0; // index iterator
	uint32_t itNormal = 0; // normal iterator
	rewind(fp);
	for (fgets(line, 100, fp); !feof(fp); fgets(line, 100, fp))
	{
		if (line[0] == 'v')
		{
			// if this line is vertex position
			if (line[1] == ' ')
			{
				glm::vec3 vertex;
				if (sscanf(line + 2, "%f%f%f", &vertex.x, &vertex.y, &vertex.z) == 3)
				{
					vertices[itVertex].position = vertex;
					itVertex++;
				}
			}
			// if this line is vertex normal
			if (line[1] == 'n')
			{
				glm::vec3 normal;
				if (sscanf(line + 2, "%f%f%f", &normal.x, &normal.y, &normal.z) == 3)
				{
					normals[itNormal] = normal;
					itNormal++;
				}
			}
		}
		// if is face (for indices)
		if (line[0] == 'f' && line[1] == ' ')
		{
			uint32_t idx[4];
			uint32_t normIdx[4];
			int8_t elementsRead = sscanf(line + 2, "%u/%*u/%u %u/%*u/%u %u/%*u/%u %u/%*u/%u", &idx[0], &normIdx[0], &idx[1], &normIdx[1], &idx[2], &normIdx[2], &idx[3], &normIdx[3]);
			//printf("%d\n", elementsRead);
			if (elementsRead > 0)
			{
				for (uint8_t i = 0; i < elementsRead / 2; i++)
				{
					indices[itIndex + i] = idx[i] - 1;
					glm::vec3 nigger = normals[normIdx[i] - 1];
					vertices[indices[itIndex + i]].normal = nigger;
				}
				itIndex += elementsRead / 2;
			}
		}
	}

	free(normals);

	for (int i = 0; i < vertCount + 1; i++)
	{
		glm::vec3 x = vertices[i].normal;
		//printf("norm %f %f %f\n", x.x, x.y, x.z);
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

Mesh::Mesh(Shader* _shader, Texture* _textures, uint8_t _texCount)
{
	shader = *_shader;
	vertex_t verts[] = {
		  // positions					   // normals                      // texture coords
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(0.0f, 0.0f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec2(0.0f, 0.0f) },
																		  
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(0.0f, 0.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(1.0f, 0.0f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f, 1.0f),   glm::vec2(0.0f, 0.0f) },
		
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
		
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(0.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(0.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(0.0f, 0.0f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
																		  
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(0.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(0.0f, 1.0f) },
																		  
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(0.0f, 1.0f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(0.0f, 1.0f) }
	};

	vertCount = sizeof(verts) / sizeof(vertex_t);
	vertices = (vertex_t*)malloc(sizeof(verts));
	if (vertices)
		memcpy(vertices, verts, sizeof(verts));

	idxCount = 0;
	indices = nullptr; // not using indexed drawing

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

	glBindVertexArray(VAO);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &vertices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, uv));

	unindexedDrawing = true;
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

void Mesh::Draw()
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (unindexedDrawing)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);
	}	
}