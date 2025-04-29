#pragma once

#include "tiny_obj_loader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Viewer {
    GLuint shader;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    tinyobj::ObjReader reader;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float clock;

    void createBuffers();
public:
    Viewer(GLuint shader);

    void init(const std::string &path);
    void update();
    void render();
};
