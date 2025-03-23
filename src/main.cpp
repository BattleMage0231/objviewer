#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <exception>
#include "viewer.h"

Viewer *viewer;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    viewer->resize(width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "objviewer", NULL, NULL);
    if(window == NULL) {
        std::cerr << "window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "failed to initialize GLAD" << std::endl;
        return -1;
    }

    viewer = new Viewer {window};

    while(!glfwWindowShouldClose(window)) {
        viewer->handleEvents();
        viewer->update();
        viewer->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete viewer;

    glfwTerminate();
    return 0;
}
