#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Viewer::Viewer(GLFWwindow* window, GLuint shader) : shader {shader}, window {window} {}

void Viewer::createBuffers() {
    std::vector<float> vertexBuffer;
    std::vector<unsigned int> elementBuffer;

    for(const auto &face : mesh.faces) {
        for(const auto &vtxIdx : face.vertices) {
            glm::vec3 vtx = mesh.vertices[vtxIdx];
            for(size_t i = 0; i < 3; ++i) vertexBuffer.emplace_back(vtx[i]);
            vertexBuffer.emplace_back(static_cast<float>(face.material));
        }
        for(size_t i = 0; i < 3; ++i) elementBuffer.emplace_back(elementBuffer.size());
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexBuffer.size(), vertexBuffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elementBuffer.size(), elementBuffer.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void Viewer::init(const std::string &path, const std::string &mtlDir) {
    mesh.load(path, mtlDir);
    createBuffers();

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera = Camera(glm::vec3 {0, 0, 0}, glm::radians(-90.0f), glm::radians(0.0f), glm::radians(60.0f), 15.0f, 4.0f);
    clock = glfwGetTime();
}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;

    if(window.isKeyPressed(GLFW_KEY_W)) camera.moveX(deltaTime);
    if(window.isKeyPressed(GLFW_KEY_S)) camera.moveX(-deltaTime);
    if(window.isKeyPressed(GLFW_KEY_D)) camera.moveY(deltaTime);
    if(window.isKeyPressed(GLFW_KEY_A)) camera.moveY(-deltaTime);

    if(window.hasLastMouseState) {
        float deltaX = 0.0, deltaY = 0.0; // temporary workaround for WSL development
        if(window.isKeyPressed(GLFW_KEY_LEFT)) deltaX -= 1;
        if(window.isKeyPressed(GLFW_KEY_RIGHT)) deltaX += 1;
        if(window.isKeyPressed(GLFW_KEY_DOWN)) deltaY -= 1;
        if(window.isKeyPressed(GLFW_KEY_UP)) deltaY += 1;

        deltaX *= deltaTime;
        deltaY *= deltaTime;

        camera.moveMouse(deltaX, deltaY);
    }

    clock = now;
}

void Viewer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glUniform3fv(glGetUniformLocation(shader, "matKd"), mesh.materialKdUniform.size(), mesh.materialKdUniform.data());
    glUniform1fv(glGetUniformLocation(shader, "matD"), mesh.materialDUniform.size(), mesh.materialDUniform.data());
    
    glm::mat4 projection = camera.getProjection(window.getAspectRatio());
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = camera.getView();
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3 * mesh.faces.size(), GL_UNSIGNED_INT, 0);
}
