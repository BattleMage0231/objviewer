#pragma once

#include "tiny_obj_loader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"

class Viewer {
    GLuint shader;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    tinyobj::ObjReader reader;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float clock;
    unsigned int width;
    unsigned int height;
    Camera camera;

    float angle = 0.0; // temp

    void createBuffers();
public:
    Viewer(GLuint shader, unsigned int width, unsigned int height);

    void init(const std::string &path);
    void update();
    void render();
};
