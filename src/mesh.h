#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <string>

struct Mesh {
    struct Material {
        std::string name;
        glm::vec3 Kd;
        float d;
    };
    
    struct Face {
        std::vector<size_t> vertices;
        size_t material;
        size_t group;
    };

    std::vector<Material> materials;
    std::vector<std::string> groups;
    std::vector<glm::vec3> vertices;

    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    float radius;

    // surface view
    std::vector<Face> faces;
    std::unordered_map<size_t, std::vector<size_t>> materialToFaces;
    std::unordered_map<size_t, std::vector<size_t>> groupToFaces;

    // rendering
    std::vector<float> materialKdUniform;
    std::vector<float> materialDUniform;

private:
    void loadBase(const std::string &path, const std::string &mtlDir);
    void loadRendering();

public:
    void load(const std::string &path, const std::string &mtlDir);
};
