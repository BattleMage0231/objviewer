#pragma once

#include <glad/glad.h>
#include <string>

class ShaderProgram {
    unsigned int id;

public:
    ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    ~ShaderProgram();

    void use();
    void setFloat3f(const std::string &name, float v0, float v1, float v2);
};
