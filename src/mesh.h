#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

struct Mesh {
    struct Material {
        std::string name;
        glm::vec3 Ka;
        glm::vec3 Kd;
        glm::vec3 Ks;
        glm::vec3 Ke;
        float Ns;
        float d;
    };
    
    struct Face {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        glm::vec3 centroid;
        size_t material;
        size_t group;
    };

    std::vector<Material> materials;
    std::vector<std::string> groups;

    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 centroid;
    float radius;

    std::vector<Face> faces;
    std::unordered_map<size_t, std::vector<size_t>> materialToFaces;
    std::unordered_map<size_t, std::vector<size_t>> groupToFaces;
    std::vector<size_t> transparentFaces;

private:
    void loadBase(const std::string &path, const std::string &mtlDir);

public:
    void load(const std::string &path, const std::string &mtlDir);
};
