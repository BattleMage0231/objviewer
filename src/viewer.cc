#include "viewer.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <algorithm>

Viewer::Viewer(GLFWwindow* window, GLuint shader) : window {window}, shader {shader} {}

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
            glm::vec3 vtx = face.vertices[i];
            glm::vec3 norm = face.normals[i];
            for(size_t i = 0; i < 3; ++i) vertexBuffer.emplace_back(vtx[i]);
            for(size_t i = 0; i < 3; ++i) vertexBuffer.emplace_back(norm[i]);
            vertexBuffer.emplace_back(static_cast<float>(face.material));
            vertexBuffer.emplace_back(static_cast<float>(face.group));
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (7 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * opaqueBuffer.size(), opaqueBuffer.data(), GL_STATIC_DRAW);

    // transparent VAO
    glBindVertexArray(vaos[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (7 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

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
        glm::vec3 pos = camera.getPosition();
        float length1 = glm::length2(pos - mesh.faces[i1].centroid);
        float length2 = glm::length2(pos - mesh.faces[i2].centroid);
        return length1 > length2;
    }); 

    transparentBuffer.clear();
    for(auto faceIdx : faces) {
        for(size_t i = 0; i < 3; ++i) transparentBuffer.emplace_back(3 * faceIdx + i);
    }
}

int Viewer::getPanelWidth() const {
    int width = static_cast<int>(window.width * 0.14f);
    return glm::max(width, 100);
}

glm::mat4 Viewer::getProjectionMatrix() const {
    glm::vec3 cameraPos = camera.getPosition();
    float distToCentroid = glm::length(cameraPos - mesh.centroid);
    float nearClip, farClip;
    if(distToCentroid >= mesh.radius) {
        nearClip = distToCentroid - mesh.radius;
        farClip = distToCentroid + mesh.radius;
    } else {
        nearClip = 0.1f;
        farClip = glm::max(nearClip + 0.1f, 2.0f * mesh.radius);
    }
    nearClip *= 0.9f;
    farClip = glm::max(nearClip, farClip) + 0.1f;
    float aspectRatio = (static_cast<float>(window.width) - 2 * getPanelWidth()) / window.height;
    return glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
}

void Viewer::raytraceMouseClick(float viewerX, float viewerY) {
    glm::vec2 mouseNDC(
        (2.0f * viewerX) / (window.width - 2 * getPanelWidth()) - 1.0f,
        1.0f - (2.0f * viewerY) / window.height
    );
    glm::vec4 rayClip = glm::vec4(mouseNDC.x, mouseNDC.y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(getProjectionMatrix()) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(camera.getView()) * rayEye));

    glm::vec3 cameraPos = camera.getPosition();
    std::optional<size_t> minIntersect = {};
    float minT = 1.0e9;
    for(const auto &face : mesh.faces) {
        if(!isGroupVisible[face.group]) continue;

        glm::vec2 baryPos;
        float t;
        if(glm::intersectRayTriangle(cameraPos, rayWorld, face.vertices[0], face.vertices[1], face.vertices[2], baryPos, t)) {
            if(t > 0.0 && t < minT) {
                minT = t;
                minIntersect = face.group;
            }
        }
    }

    if(!minIntersect) {
        selectedGroup = -1;
    } else {
        selectedGroup = *minIntersect;
    }
}

void Viewer::init(const std::string &path, const std::string &mtlDir) {
    mesh.load(path, mtlDir);
    createData();
    createBuffers();

    camera = Camera(mesh.centroid, mesh.radius * 2.0f, 0.0f, 0.0f);

    selectedGroup = -1;
    isGroupVisible = std::vector<int>(mesh.groups.size(), 1);
    fovDegrees = 60.0f;
    rotateSpeed = 2.0f;
    zoomSpeed = 1.0f;
    ambientLighting = 1.0f;
    diffuseLighting = 2.0f;
    specularLighting = 2.0f;
    emissiveLighting = 1.0f;
    applyGamma = true;
    fps = 0.0f;

    framesSinceLastUpdate = 0;
    lastUpdateTime = 0.0f;
    clock = glfwGetTime();
}

void Viewer::update() {
    float now = glfwGetTime();
    float deltaTime = now - clock;

    if(window.mouseLeftPressed) {
        int panelWidth = getPanelWidth();
        float mouseX = window.mouseX;
        float mouseY = window.mouseY;
        if(panelWidth <= mouseX && mouseX <= window.width - panelWidth) {
            raytraceMouseClick(mouseX - panelWidth, mouseY);
        }
    }

    float deltaX = 0.0f, deltaY = 0.0f, deltaZoom = 0.0f;
    if(window.isKeyPressed(GLFW_KEY_LEFT)) deltaX -= 1;
    if(window.isKeyPressed(GLFW_KEY_RIGHT)) deltaX += 1;
    if(window.isKeyPressed(GLFW_KEY_DOWN)) deltaY -= 1;
    if(window.isKeyPressed(GLFW_KEY_UP)) deltaY += 1;

    if(window.isKeyPressed(GLFW_KEY_Z)) deltaZoom -= 1;
    else if(window.mouseScrollY > 0.0f) deltaZoom -= 4.0f * window.mouseScrollY;

    if(window.isKeyPressed(GLFW_KEY_X)) deltaZoom += 1;
    else if(window.mouseScrollY < 0.0f) deltaZoom -= 4.0f * window.mouseScrollY;

    window.mouseScrollY = 0.0f;
    window.mouseLeftPressed = false;
    
    deltaX *= deltaTime;
    deltaY *= deltaTime;
    deltaZoom *= deltaTime;
    camera.rotate(deltaX * rotateSpeed, deltaY * rotateSpeed);
    camera.zoom(deltaZoom * zoomSpeed * mesh.radius);

    if(now - lastUpdateTime > 0.5f) {
        fps = framesSinceLastUpdate / (now - lastUpdateTime);
        framesSinceLastUpdate = 0;
        lastUpdateTime = now;
    } else {
        ++framesSinceLastUpdate;
    }

    clock = now;
}

void Viewer::renderUI() {
    int fbWidth = window.width;
    int fbHeight = window.height;
    int panelWidth = getPanelWidth();

    // left panel
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float) panelWidth, (float) ImGui::GetIO().DisplaySize.y));
    ImGui::Begin("Left Panel", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Groups");

    auto count = std::count(isGroupVisible.cbegin(), isGroupVisible.cend(), 1);
    if(ImGui::Button(count > 0 ? "Hide All" : "Show All")) {
        if(count > 0) {
            std::fill(isGroupVisible.begin(), isGroupVisible.end(), 0);
        } else {
            std::fill(isGroupVisible.begin(), isGroupVisible.end(), 1);
        }
    }

    for(size_t i = 0; i < mesh.groups.size(); ++i) {
        ImGui::PushID(i); 

        ImVec4 color = (isGroupVisible[i] == 1) 
            ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
            : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

        if(ImGui::Button("##visBtn", ImVec2(20, 0))) {
            isGroupVisible[i] = 1 - isGroupVisible[i];
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine(40);
        
        bool isSelected = (selectedGroup == static_cast<int>(i));
        if(ImGui::Selectable(mesh.groups[i].c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
            selectedGroup = isSelected ? -1 : i;
        }

        ImGui::PopID();
    }

    ImGui::End();

    // right panel
    ImGui::SetNextWindowPos(ImVec2((float) (fbWidth - panelWidth), 0));
    ImGui::SetNextWindowSize(ImVec2((float) panelWidth, (float) fbHeight));
    ImGui::Begin("Right Panel", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Settings & Information");

    if(selectedGroup != -1 && ImGui::CollapsingHeader("Selected Group", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Name: %s", mesh.groups[selectedGroup].c_str());

        int numberOfFaces = mesh.groupToFaces[static_cast<size_t>(selectedGroup)].size();
        ImGui::Text("Number of Faces: %d", numberOfFaces);
    }
    
    if(ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FOV");
        ImGui::SliderFloat("##fov", &fovDegrees, 10.0f, 90.0f);

        ImGui::Text("Rotation Speed");
        ImGui::SliderFloat("##rotSpeed", &rotateSpeed, 0.5f, 5.0f);

        ImGui::Text("Zoom Speed");
        ImGui::SliderFloat("##zoomSpeed", &zoomSpeed, 0.1f, 2.0f);
    }

    if(ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Ambient Strength");
        ImGui::SliderFloat("##ambientStrength", &ambientLighting, 0.0f, 2.0f);

        ImGui::Text("Diffuse Strength");
        ImGui::SliderFloat("##diffuseStrength", &diffuseLighting, 0.0f, 3.0f);

        ImGui::Text("Specular Strength");
        ImGui::SliderFloat("##specularStrength", &specularLighting, 0.0f, 3.0f);

        ImGui::Text("Emissive Strength");
        ImGui::SliderFloat("##emissiveStrength", &emissiveLighting, 0.0f, 2.0f);

        ImGui::Checkbox("Gamma Correction", &applyGamma);
    }

    if(ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FPS: %.1f", fps);
    }

    ImGui::End();
}

void Viewer::renderViewer() {
    glViewport(getPanelWidth(), 0, window.width - 2 * getPanelWidth(), window.height);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    
    glm::mat4 projection = getProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = camera.getView();
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);

    glUniform3fv(glGetUniformLocation(shader, "matKa"), materialKaUniform.size(), materialKaUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKd"), materialKdUniform.size(), materialKdUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKs"), materialKsUniform.size(), materialKsUniform.data());
    glUniform3fv(glGetUniformLocation(shader, "matKe"), materialKeUniform.size(), materialKeUniform.data());
    glUniform1fv(glGetUniformLocation(shader, "matNs"), materialNsUniform.size(), materialNsUniform.data());
    glUniform1fv(glGetUniformLocation(shader, "matD"), materialDUniform.size(), materialDUniform.data());

    glUniform1iv(glGetUniformLocation(shader, "groupVisibility"), isGroupVisible.size(), isGroupVisible.data());
    glUniform1i(glGetUniformLocation(shader, "selectedGroup"), selectedGroup);
    glUniform1f(glGetUniformLocation(shader, "ambientStrength"), ambientLighting);
    glUniform1f(glGetUniformLocation(shader, "diffuseStrength"), diffuseLighting);
    glUniform1f(glGetUniformLocation(shader, "specularStrength"), specularLighting);
    glUniform1f(glGetUniformLocation(shader, "emissiveStrength"), emissiveLighting);
    glUniform1i(glGetUniformLocation(shader, "applyGamma"), applyGamma);

    glm::vec3 cameraPos = camera.getPosition();
    glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
    
    glm::vec3 lightPos = cameraPos;
    glUniform3f(glGetUniformLocation(shader, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);

    glEnable(GL_DEPTH_TEST);

    // draw opaque
    glBindVertexArray(opaqueVao);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glDrawElements(GL_TRIANGLES, opaqueBuffer.size(), GL_UNSIGNED_INT, 0);

    // draw transparent
    sortTransparentFaces();

    glBindVertexArray(transparentVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * transparentBuffer.size(), transparentBuffer.data(), GL_STREAM_DRAW);
    
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, transparentBuffer.size(), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
}

void Viewer::render() {
    renderUI();
    renderViewer();
}
