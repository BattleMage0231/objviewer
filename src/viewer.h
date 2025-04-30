#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "window.h"
#include "mesh.h"

class Viewer {
    GLuint shader;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    float clock;

    Mesh mesh;
    Camera camera;
    Window window;

    void createBuffers();
public:
    Viewer(GLFWwindow* window, GLuint shader);

    void init(const std::string &path, const std::string &mtlDir);
    void update();
    void render();
};
