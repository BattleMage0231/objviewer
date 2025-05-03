#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Viewer::Viewer(GLFWwindow* window, GLuint shader) : shader {shader}, window {window} {}

void Viewer::createData() {
    materialKaUniform.clear();
    materialKdUniform.clear();
    materialKsUniform.clear();
    materialKeUniform.clear();
    materialNsUniform.clear();
    materialDUniform.clear();
    vertexBuffer.clear();
    opaqueBuffer.clear();
    transparentBuffer.clear();

    materialKaUniform.reserve(3 * mesh.materials.size());
    materialKdUniform.reserve(3 * mesh.materials.size());
    materialKsUniform.reserve(3 * mesh.materials.size());
    materialKeUniform.reserve(3 * mesh.materials.size());
    materialNsUniform.reserve(mesh.materials.size());
    materialDUniform.reserve(mesh.materials.size());

    for(size_t i = 0; i < mesh.materials.size(); ++i) {
        for(size_t j = 0; j < 3; ++j) {
            materialKaUniform.emplace_back(mesh.materials[i].Ka[j]);
            materialKdUniform.emplace_back(mesh.materials[i].Kd[j]);
            materialKsUniform.emplace_back(mesh.materials[i].Ks[j]);
            materialKeUniform.emplace_back(mesh.materials[i].Ke[j]);
        }
        materialNsUniform.emplace_back(mesh.materials[i].Ns);
        materialDUniform.emplace_back(mesh.materials[i].d);
    }

    size_t vtxCnt = 0;
    for(const auto &face : mesh.faces) {
        for(size_t i = 0; i < face.vertices.size(); ++i) {
            glm::vec3 vtx = mesh.vertices[face.vertices[i]];
            glm::vec3 norm = face.normals[i];
            for(size_t i = 0; i < 3; ++i) vertexBuffer.emplace_back(vtx[i]);
            for(size_t i = 0; i < 3; ++i) vertexBuffer.emplace_back(norm[i]);
            vertexBuffer.emplace_back(static_cast<float>(face.material));
            ++vtxCnt;
        }
        auto &buffer = mesh.materials[face.material].d < 1.0 ? transparentBuffer : opaqueBuffer;
        for(size_t i = 0; i < 3; ++i) buffer.emplace_back(vtxCnt - 3 + i);
    }
}

void Viewer::createBuffers() {
    unsigned int vbo, vaos[2], ebos[2];

    glGenVertexArrays(2, vaos);
    glGenBuffers(1, &vbo);
    glGenBuffers(2, ebos);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexBuffer.size(), vertexBuffer.data(), GL_STATIC_DRAW);

    // opaque VAO
    glBindVertexArray(vaos[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * opaqueBuffer.size(), opaqueBuffer.data(), GL_STATIC_DRAW);

    // transparent VAO
    glBindVertexArray(vaos[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * transparentBuffer.size(), transparentBuffer.data(), GL_STREAM_DRAW);

    glBindVertexArray(0);

    opaqueVao = vaos[0];
    transparentVao = vaos[1];
    transparentEbo = ebos[1];
}

void Viewer::sortTransparentFaces() {
    std::vector<size_t> faces = mesh.transparentFaces;
    std::sort(faces.begin(), faces.end(), [&](auto i1, auto i2) {
        const auto &face1 = mesh.faces[i1];
        const auto &face2 = mesh.faces[i2];
        glm::vec3 centroid1 = (mesh.vertices[face1.vertices[0]] + mesh.vertices[face1.vertices[1]] + mesh.vertices[face1.vertices[2]]) / 3.0f;
        glm::vec3 centroid2 = (mesh.vertices[face2.vertices[0]] + mesh.vertices[face2.vertices[1]] + mesh.vertices[face2.vertices[2]]) / 3.0f;
        return glm::length(camera.getPosition() - centroid1) > glm::length(camera.getPosition() - centroid2);
    }); 

    transparentBuffer.clear();
    for(auto faceIdx : faces) {
        for(size_t i = 0; i < 3; ++i) transparentBuffer.emplace_back(3 * faceIdx + i);
    }
}

void Viewer::init(const std::string &path, const std::string &mtlDir) {
    mesh.load(path, mtlDir);
    createData();
    createBuffers();

    camera = Camera(mesh.center, mesh.radius * 1.15f, 0.0f, 0.0f, glm::radians(60.0f));

    clock = glfwGetTime();
}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;

    float deltaX = 0.0f, deltaY = 0.0f, deltaZoom = 0.0f;
    if(window.isKeyPressed(GLFW_KEY_LEFT)) deltaX -= 1;
    if(window.isKeyPressed(GLFW_KEY_RIGHT)) deltaX += 1;
    if(window.isKeyPressed(GLFW_KEY_DOWN)) deltaY -= 1;
    if(window.isKeyPressed(GLFW_KEY_UP)) deltaY += 1;
    if(window.isKeyPressed(GLFW_KEY_Z)) deltaZoom -= 1;
    if(window.isKeyPressed(GLFW_KEY_X)) deltaZoom += 1;
    
    deltaX *= deltaTime;
    deltaY *= deltaTime;
    deltaZoom *= deltaTime;
    camera.rotate(deltaX * 2.0f, deltaY * 2.0f);
    camera.zoom(deltaZoom * 0.4 * mesh.radius);

    clock = now;
}

void Viewer::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    
    glm::mat4 projection = camera.getProjection(window.getAspectRatio());
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = camera.getView();
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);

    glUniform3fv(glGetUniformLocation(shader, "matKa"), materialKaUniform.size(), materialKaUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKd"), materialKdUniform.size(), materialKdUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKs"), materialKsUniform.size(), materialKsUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKe"), materialKeUniform.size(), materialKeUniform.data());
    glUniform1fv(glGetUniformLocation(shader, "matNs"), materialNsUniform.size(), materialNsUniform.data());
    glUniform1fv(glGetUniformLocation(shader, "matD"), materialDUniform.size(), materialDUniform.data());

    glm::vec3 cameraPos = camera.getPosition();
    glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
    glUniform3f(glGetUniformLocation(shader, "lightPos"), cameraPos[0], cameraPos[1], cameraPos[2]);

    glEnable(GL_DEPTH_TEST);

    // draw opaque
    glBindVertexArray(opaqueVao);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glDrawElements(GL_TRIANGLES, opaqueBuffer.size(), GL_UNSIGNED_INT, 0);

    // draw transparent
    sortTransparentFaces();

    glBindVertexArray(transparentVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * transparentBuffer.size(), transparentBuffer.data(), GL_STREAM_DRAW);
    
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    glDrawElements(GL_TRIANGLES, transparentBuffer.size(), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
}
