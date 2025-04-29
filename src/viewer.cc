#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Viewer::Viewer(GLuint shader, unsigned int width, unsigned int height) : shader {shader}, width {width}, height {height} {}

void Viewer::createBuffers() {
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    vertices = attrib.vertices;
    indices.clear();

    for(size_t s = 0; s < shapes.size(); ++s) {
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            if(fv == 3) {
                for(size_t v = 0; v < fv; ++v) {
                    unsigned int idx = shapes[s].mesh.indices[index_offset + v].vertex_index;
                    indices.emplace_back(idx);
                }
            }
            index_offset += fv;
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
}

void Viewer::init(const std::string &path) {
    tinyobj::ObjReaderConfig config;
   
    if(!reader.ParseFromFile(path, config)) {
        if(!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        throw std::runtime_error {""};
    }

    createBuffers();

    glEnable(GL_DEPTH_TEST);

    camera = Camera {glm::vec3 {4, 4, 4}, glm::vec3 {-1, -1, -1}};
    clock = glfwGetTime();
}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;

    angle += deltaTime;
    camera.update(angle);

    clock = now;
}

void Viewer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glUniform4f(glGetUniformLocation(shader, "inputColor"), 1.0f, 1.0f, 1.0f, 1.0f);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / height, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = camera.getView();
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
