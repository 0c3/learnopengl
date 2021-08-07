#include "pch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using shader_internal::uniform_info_t;

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    FILE* fp;
    long length;
    char* buffer;
    
    ////////////////////
    // VERTEX SHADER //
    //////////////////

    fp = fopen(vertexPath, "rb");
    if (!fp)
    {
        char* str = (char*)malloc(12 + strlen(vertexPath));
        if (str)
        {
            strcpy(str, "res/shader/");
            fp = fopen(strcat(str, vertexPath), "rb");
            free(str);
        }
        if (!fp)
            perror(vertexPath), exit(1);
    }

    // get file length
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    // allocate memory for entire content
    buffer = (char*)malloc((size_t)length + 1);
    if (!buffer) 
        fclose(fp), fputs("failed to allocate vertex shader buffer!", stderr), exit(1);

    // copy the file into the buffer
    if (fread(buffer, length, 1, fp) != 1) // if file read fails
        fclose(fp), free(buffer), fputs("failed to read vertex shader!", stderr), exit(1);
    fclose(fp);
    buffer[length] = '\0'; // zero terminate

    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &buffer, NULL); // pass NULL as final argument cause i zero terminated it before
    glCompileShader(vertex);

    // get & print compile errors if any
    int32_t success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("vertex shader compilation failed!\n%s\n", infoLog);
    }

    free(buffer);

    //////////////////////
    // FRAGMENT SHADER //
    ////////////////////

    fp = fopen(fragmentPath, "rb");
    if (!fp)
    {
        char* str = (char*)malloc(12 + strlen(fragmentPath));
        if (str)
        {
            strcpy(str, "res/shader/");
            fp = fopen(strcat(str, fragmentPath), "rb");
            free(str);
        }
        if (!fp)
            perror(fragmentPath), exit(1);
    }

    // get file length
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    // allocate memory for entire content
    buffer = (char*)malloc((size_t)length + 1);
    if (!buffer)
        fclose(fp), fputs("failed to allocate vertex shader buffer!", stderr), exit(1);

    // copy the file into the buffer
    if (fread(buffer, length, 1, fp) != 1) // if file read fails
        fclose(fp), free(buffer), fputs("failed to read vertex shader!", stderr), exit(1);
    fclose(fp);
    buffer[length] = '\0'; // zero terminate

    uint32_t frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &buffer, NULL); // pass NULL as final argument cause i zero terminated it before
    glCompileShader(frag);
    // print compile erros
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        printf("fragment shader compilation failed!\n%s\n", infoLog);
    }

    free(buffer);

    /////////////////////
    // SHADER PROGRAM //
    ///////////////////

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    // print likngin errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("shader program linkage failed!\n%s\n", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);

    // retrieve and cache all uniforms
    int32_t uniformCount;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformCount);

    if (uniformCount != 0)
    {
        int32_t maxNameLen = 0;
        glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);

        char* uniformName = (char*)malloc(maxNameLen);

        uint32_t type = GL_NONE;

        for (int32_t i = 0, count = 0, length = 0; i < uniformCount; i++)
        {
            glGetActiveUniform(ID, i, maxNameLen, &length, &count, &type, uniformName);

            uniform_info_t uniformInfo = {
                glGetUniformLocation(ID, uniformName), // location
                count                                  // count
            };

            uniforms.emplace(fnv::hash(uniformName), uniformInfo);
        }
    }
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

bool Shader::Reload(const char* vertexPath, const char* fragmentPath)
{
    FILE* fp;
    long length;
    char* buffer;

    ////////////////////
    // VERTEX SHADER //
    //////////////////

    fp = fopen(vertexPath, "rb");
    if (!fp)
    {
        char* str = (char*)malloc(13 + strlen(vertexPath));
        if (str)
        {
            strcpy(str, "res/shaders/");
            fp = fopen(strcat(str, vertexPath), "rb");
            free(str);
        }
        if (!fp)
        {
            perror(vertexPath);
            return false;
        }
    }

    // get file length
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    // allocate memory for entire content
    buffer = (char*)malloc((size_t)length + 1);
    if (!buffer)
        fclose(fp), fputs("failed to allocate vertex shader buffer!", stderr), exit(1);

    // copy the file into the buffer
    if (fread(buffer, length, 1, fp) != 1) // if file read fails
        fclose(fp), free(buffer), fputs("failed to read vertex shader!", stderr), exit(1);
    fclose(fp);
    buffer[length] = '\0'; // zero terminate

    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &buffer, NULL); // pass NULL as final argument cause i zero terminated it before
    glCompileShader(vertex);

    // get & print compile errors if any
    int32_t success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("vertex shader compilation failed!\n%s\n", infoLog);
        return false;
    }

    free(buffer);

    //////////////////////
    // FRAGMENT SHADER //
    ////////////////////

    fp = fopen(fragmentPath, "rb");
    if (!fp)
    {
        char* str = (char*)malloc(13 + strlen(fragmentPath));
        if (str)
        {
            strcpy(str, "res/shaders/");
            fp = fopen(strcat(str, fragmentPath), "rb");
            free(str);
        }
        if (!fp)
        {
            perror(fragmentPath);
            return false;
        }
    }

    // get file length
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    // allocate memory for entire content
    buffer = (char*)malloc((size_t)length + 1);
    if (!buffer)
        fclose(fp), fputs("failed to allocate vertex shader buffer!", stderr), exit(1);

    // copy the file into the buffer
    if (fread(buffer, length, 1, fp) != 1) // if file read fails
        fclose(fp), free(buffer), fputs("failed to read vertex shader!", stderr), exit(1);
    fclose(fp);
    buffer[length] = '\0'; // zero terminate

    uint32_t frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &buffer, NULL); // pass NULL as final argument cause i zero terminated it before
    glCompileShader(frag);
    // print compile erros
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        printf("fragment shader compilation failed!\n%s\n", infoLog);
        return false;
    }

    free(buffer);

    /////////////////////
    // SHADER PROGRAM //
    ///////////////////

    glDeleteProgram(ID);
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    // print likngin errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("shader program linkage failed!\n%s\n", infoLog);
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);

    // clear old uniforms
    uniforms.clear();

    // retrieve and cache all uniforms
    int32_t uniformCount;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformCount);

    if (uniformCount != 0)
    {
        int32_t maxNameLen = 0;
        glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);

        char* uniformName = (char*)malloc(maxNameLen);

        uint32_t type = GL_NONE;

        for (int32_t i = 0, count = 0, length = 0; i < uniformCount; i++)
        {
            glGetActiveUniform(ID, i, maxNameLen, &length, &count, &type, uniformName);

            uniform_info_t uniformInfo = {
                glGetUniformLocation(ID, uniformName), // location
                count                                  // count
            };

            uniforms.emplace(fnv::hash(uniformName), uniformInfo);
        }
    }
    return true;
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::SetValue(const char* name, bool value)
{
	glUniform1i(uniforms[fnv::hash(name)].location, (int32_t)value);
}

void Shader::SetValue(const char* name, int32_t value)
{
    /*uniform_info_t* info = &uniforms[fnv::hash(name)];
    if (info->count != 0)
        glUniform1i(info->location, value);*/
    glUniform1i(uniforms[fnv::hash(name)].location, value);
}

void Shader::SetValue(const char* name, uint32_t value)
{
    glUniform1ui(uniforms[fnv::hash(name)].location, value);
}

void Shader::SetValue(const char* name, float value)
{
	glUniform1f(uniforms[fnv::hash(name)].location, value);
}

// vectors

void Shader::SetValue(const char* name, const glm::vec2& value)
{
    glUniform2fv(uniforms[fnv::hash(name)].location, 1, &value[0]);
}

void Shader::SetValue(const char* name, float x, float y)
{
    glUniform2f(uniforms[fnv::hash(name)].location, x, y);
}

void Shader::SetValue(const char* name, const glm::vec3& value)
{
    glUniform3fv(uniforms[fnv::hash(name)].location, 1, &value[0]);
}

void Shader::SetValue(const char* name, float x, float y, float z)
{
    glUniform3f(uniforms[fnv::hash(name)].location, x, y, z);
}

void Shader::SetValue(const char* name, const glm::vec4& value)
{
    glUniform4fv(uniforms[fnv::hash(name)].location, 1, &value[0]);
}

void Shader::SetValue(const char* name, float x, float y, float z, float w)
{
    glUniform4f(uniforms[fnv::hash(name)].location, x, y, z, w);
}

// matrices
void Shader::SetValue(const char* name, const glm::mat2& value)
{
    glUniformMatrix2fv(uniforms[fnv::hash(name)].location, 1, GL_FALSE, &value[0][0]);
}

void Shader::SetValue(const char* name, const glm::mat3& value)
{
    glUniformMatrix3fv(uniforms[fnv::hash(name)].location, 1, GL_FALSE, &value[0][0]);
}

void Shader::SetValue(const char* name, const glm::mat4& value)
{
    glUniformMatrix4fv(uniforms[fnv::hash(name)].location, 1, GL_FALSE, &value[0][0]);
}

void Shader::SetArray(const char* name, float value)
{
    uniform_info_t info = uniforms[fnv::hash(name)];
    size_t bytes = sizeof(float) * info.count;
    float* data = (float*)malloc(bytes);
    if (data)
    {
        memset(data, value, bytes);
        glUniform1fv(info.location, info.count, data);
        free(data);
    }
    else
        printf("failed setting array: %s - data pointer was NULL\n", name);
}
