#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "window.h"
#include "mesh.h"

class Viewer {
    GLuint shader;
    GLuint opaqueVao;
    GLuint transparentVao;
    GLuint transparentEbo;

    std::vector<float> materialKaUniform;
    std::vector<float> materialKdUniform;
    std::vector<float> materialKsUniform;
    std::vector<float> materialKeUniform;
    std::vector<float> materialNsUniform;
    std::vector<float> materialDUniform;
    std::vector<float> vertexBuffer;
    std::vector<unsigned int> opaqueBuffer;
    std::vector<unsigned int> transparentBuffer;

    Mesh mesh;
    Camera camera;
    Window window;

    float clock;

    std::optional<size_t> selectedGroup;
    std::vector<int> isGroupVisible;

    void createData();
    void createBuffers();

    void sortTransparentFaces();

    int getPanelWidth() const;
    glm::mat4 getProjectionMatrix() const;

    void renderUI();
    void renderViewer();
public:
    Viewer(GLFWwindow* window, GLuint shader);

    void init(const std::string &path, const std::string &mtlDir);
    void update();
    void render();
};
