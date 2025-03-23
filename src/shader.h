#pragma once

#include <glad/glad.h>
#include <string>

class ShaderProgram {
    unsigned int id;

public:
    ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    ~ShaderProgram();

    void use();
};
