#pragma once

#include "shader.h"
#include "parser.h"
#include <GLFW/glfw3.h>

class Viewer {
    GLFWwindow *window;
    float clock;
    ObjData data;
    ShaderProgram shader;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

public:
    Viewer(GLFWwindow *window);
    ~Viewer();

    void init();
    void resize(int width, int height);
    void handleEvents();
    void update();
    void render();
};
