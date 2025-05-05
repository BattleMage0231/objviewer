#include "window.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <iostream>

static void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleMouseEvent(xPos, yPos);
    ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleKeyEvent(key, action);
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleFramebufferSizeEvent(width, height);
}

Window::Window(GLFWwindow *window) {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetWindowSize(window, &width, &height);
}

void Window::update() {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void Window::handleMouseEvent(float xPos, float yPos) {
    mouseX = xPos;
    mouseY = yPos;
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
