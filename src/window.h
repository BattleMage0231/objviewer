#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

struct Window {
    std::unordered_map<int, bool> keyStates;
    float mouseX;
    float mouseY;
    float lastMouseX;
    float lastMouseY;
    int width;
    int height;

    Window(GLFWwindow* window);

    void update();
    void handleMouseEvent(float xPos, float yPos);
    void handleKeyEvent(int key, int action);
    void handleFramebufferSizeEvent(int width, int height);
    bool isKeyPressed(int key) const;
};
