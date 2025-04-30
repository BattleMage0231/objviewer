#include "mesh.h"
#include "tiny_obj_loader.h"
#include <iostream>

void Mesh::loadBase(const std::string &path, const std::string &mtlDir) {
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = mtlDir;
   
    if(!reader.ParseFromFile(path, config)) {
        if(!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        throw std::runtime_error {""};
    }

    const auto &objVertices = reader.GetAttrib().GetVertices();
    const auto& objShapes = reader.GetShapes();
    const auto& objMaterials = reader.GetMaterials();

    materials.emplace_back(Material {"none", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f});
    for(const auto &mat : objMaterials) {
        glm::vec3 Kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
        materials.emplace_back(Material {mat.name, Kd, mat.dissolve });
    }

    min = glm::vec3(1.0e9, 1.0e9, 1.0e9);
    max = glm::vec3(-1.0e9, -1.0e9, -1.0e9);
    for(size_t i = 0; i < objVertices.size(); i += 3) {
        vertices.emplace_back(objVertices[i], objVertices[i + 1], objVertices[i + 2]);
        min = glm::min(min, vertices.back());
        max = glm::max(max, vertices.back());
    }
    center = (min + max) * 0.5f;
    radius = glm::max(glm::length(max - center), glm::length(min - center));

    for(const auto &shape : objShapes) {
        groups.emplace_back(shape.name);

        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            Face face;
            face.material = static_cast<size_t>(shape.mesh.material_ids[f] + 1);
            face.group = groups.size() - 1;

            for(size_t v = 0; v < 3; ++v) {
                size_t vertexIndex = static_cast<size_t>(shape.mesh.indices[3 * f + v].vertex_index);
                face.vertices.emplace_back(vertexIndex);
            }

            faces.emplace_back(std::move(face));
            materialToFaces[face.material].emplace_back(faces.size() - 1);
            groupToFaces[face.group].emplace_back(faces.size() - 1);
        }
    }
}

void Mesh::loadRendering() {
    materialKdUniform.reserve(3 * materials.size());
    materialDUniform.reserve(materials.size());
    for(size_t i = 0; i < materials.size(); ++i) {
        for(size_t j = 0; j < 3; ++j) materialKdUniform.emplace_back(materials[i].Kd[j]);
        materialDUniform.emplace_back(materials[i].d);
    }
}

void Mesh::load(const std::string &path, const std::string &mtlDir) {
    materials.clear();
    groups.clear();
    vertices.clear();
    faces.clear();
    materialToFaces.clear();
    groupToFaces.clear();
    materialKdUniform.clear();
    materialDUniform.clear();

    loadBase(path, mtlDir);
    loadRendering();
}
