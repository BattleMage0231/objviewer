#include "viewer.h"
#include <iostream>

Viewer::Viewer(GLuint shader) : shader {shader} {}

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

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

    clock = glfwGetTime();
}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;
    clock = now;
}

void Viewer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "inputColor"), 0.0f, 0.0f, 0.0f);
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT);
}
