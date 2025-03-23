#include "viewer.h"
#include <glm/glm.hpp>

Viewer::Viewer(GLFWwindow *window) : window {window} {}

void Viewer::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void Viewer::init() {
    clock = glfwGetTime();
}
    
void Viewer::handleEvents() {}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;
    clock = now;

    for(size_t i = 0; i < 3; ++i) {
        color[i] = 0.5 + 0.5 * glm::cos(i * now);
    }
}

void Viewer::render() {
    glClearColor(color[0], color[1], color[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
