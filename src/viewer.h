#pragma once

#include "shader.h"
#include <GLFW/glfw3.h>

class Viewer {
    GLFWwindow *window;
    float clock;
    float color[3];
    //ShaderProgram program;

public:
    Viewer(GLFWwindow *window);

    void init();
    void resize(int width, int height);
    void handleEvents();
    void update();
    void render();
};
