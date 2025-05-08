#include "window.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <iostream>

static void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleMouseEvent(xPos, yPos);
    ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleMouseButton(button, action);
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

static void scrollCallback(GLFWwindow* window, double xDelta, double yDelta) {
    Window* c = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(c) c->handleMouseScroll(yDelta);
    ImGui_ImplGlfw_ScrollCallback(window, xDelta, yDelta);
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

Window::Window(GLFWwindow *window) : 
    mouseScrollY {0.0f}, 
    mouseLeftDown {false},
    mouseReleaseReceived {false}
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwGetWindowSize(window, &width, &height);
}

void Window::newMouseMotionSegment() {
    if(mouseLeftDown) {
        mouseSegmentStartX = mouseX;
        mouseSegmentStartY = mouseY;
    }
}

void Window::handleMouseEvent(float xPos, float yPos) {
    mouseX = xPos;
    mouseY = yPos;
}

void Window::handleMouseButton(int button, int action) {
    if(button != GLFW_MOUSE_BUTTON_LEFT) return;

    if(action == GLFW_PRESS) {
        mouseMotionStartX = mouseX;
        mouseMotionStartY = mouseY;
        mouseSegmentStartX = mouseX;
        mouseSegmentStartY = mouseY;
        mouseMotionStartTime = glfwGetTime();
        mouseLeftDown = true;
    } else if(action == GLFW_RELEASE) {
        mouseReleaseReceived = true;
        mouseLeftDown = false;
    }
}

void Window::handleMouseScroll(float yDelta) {
    mouseScrollY = yDelta;
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
