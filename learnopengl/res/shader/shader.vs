#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 VP;
uniform mat4 model;
uniform mat3 normalMatrix;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = VP * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    TexCoords = aTexCoords;
}