#include "window.h"
#include <iostream>

static void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleMouseEvent(xPos, yPos);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleKeyEvent(key, action);
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleFramebufferSizeEvent(width, height);
}

Window::Window(GLFWwindow *window) : hasLastMouseState {false} {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetWindowSize(window, &width, &height);
}

void Window::handleMouseEvent(float xPos, float yPos) {
    if(!hasLastMouseState) {
        lastMouseX = xPos;
        lastMouseY = yPos;
        deltaX = 0.0f;
        deltaY = 0.0f;
        hasLastMouseState = true;
    } else {
        deltaX = xPos - lastMouseX;
        deltaY = yPos - lastMouseY;
        lastMouseX = xPos;
        lastMouseY = yPos;
    }
}

void Window::handleKeyEvent(int key, int action) {
    if(action == GLFW_PRESS) {
        keyStates[key] = true;
    } else if(action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

void Window::handleFramebufferSizeEvent(int width, int height) {
    this->width = width;
    this->height = height;
}

bool Window::isKeyPressed(int key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}

float Window::getAspectRatio() const {
    return (float) width / height;
}
