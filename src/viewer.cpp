#include "viewer.h"
#include <iostream>

Viewer::Viewer(GLFWwindow *window) : window {window}, shader {"../assets/vertex.glsl", "../assets/fragment.glsl"} {}

Viewer::~Viewer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Viewer::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void Viewer::init() {
    ObjParser parser {"../file.obj"};
    parser.parse();
    data = parser.getResult();

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFF);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(float), data.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.faces.size() * sizeof(unsigned int), data.faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    clock = glfwGetTime();
}
    
void Viewer::handleEvents() {}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;
    clock = now;
}

void Viewer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setFloat3f("inputColor", 0.0f, 0.0f, 0.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, data.faces.size(), GL_UNSIGNED_INT, 0);
}
