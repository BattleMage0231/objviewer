#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

struct Window {
    std::unordered_map<int, bool> keyStates;

    float mouseX;
    float mouseY;
    float mouseMotionStartX;
    float mouseMotionStartY;
    float mouseSegmentStartX;
    float mouseSegmentStartY;
    float mouseMotionStartTime;
    float mouseScrollY;
    bool mouseLeftDown;
    bool mouseReleaseReceived;

    int width;
    int height;

    Window(GLFWwindow* window);

    void newMouseMotionSegment();
    void handleMouseEvent(float xPos, float yPos);
    void handleMouseButton(int button, int action);
    void handleMouseScroll(float yDelta);
    void handleKeyEvent(int key, int action);
    void handleFramebufferSizeEvent(int width, int height);
    bool isKeyPressed(int key) const;
};
