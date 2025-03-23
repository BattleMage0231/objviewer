#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>

ShaderProgram::ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string paths[2] { vertexPath, fragmentPath };
    GLenum types[2] { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    unsigned int ids[2];
    int success;
    char log[512];

    for(size_t i = 0; i < 2; ++i) {
        std::ifstream s;
        s.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        s.open(paths[i]);
        std::stringstream buffer;
        buffer << s.rdbuf();
        std::string content = buffer.str();
        const char* contentRaw = content.c_str();

        ids[i] = glCreateShader(types[i]);
        glShaderSource(ids[i], 1, &contentRaw, NULL);
        glCompileShader(ids[i]);

        glGetShaderiv(ids[i], GL_COMPILE_STATUS, &success);
        if(!success) {
            for(size_t j = 0; j < i; ++j) glDeleteShader(ids[j]);

            glGetShaderInfoLog(ids[i], sizeof(log), NULL, log);
            std::cerr << "compilation of shader at path " << paths[i] << " failed with error " << log << std::endl;
            throw std::runtime_error {""};
        }
    }

    id = glCreateProgram();
    glAttachShader(id, ids[0]);
    glAttachShader(id, ids[1]);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, sizeof(log), NULL, log);
        std::cerr << "linking of program failed with error " << log << std::endl;
        throw std::runtime_error {""};
    }

    glDeleteShader(ids[0]);
    glDeleteShader(ids[1]);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::use() {
    glUseProgram(id);
}
