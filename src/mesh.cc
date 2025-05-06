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

    const auto& objAttrib = reader.GetAttrib();
    const auto& objVertices = objAttrib.vertices;
    const auto& objNormals = objAttrib.normals;
    const auto& objShapes = reader.GetShapes();
    const auto& objMaterials = reader.GetMaterials();

    materials.emplace_back(Material {"none", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f});
    for(const auto &mat : objMaterials) {
        glm::vec3 Ka(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
        glm::vec3 Kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
        glm::vec3 Ks(mat.specular[0], mat.specular[1], mat.specular[2]);
        glm::vec3 Ke(mat.emission[0], mat.emission[1], mat.emission[2]);

        materials.emplace_back(Material {mat.name, Ka, Kd, Ks, Ke, mat.shininess, mat.dissolve });
    }

    min = glm::vec3(1.0e9, 1.0e9, 1.0e9);
    max = glm::vec3(-1.0e9, -1.0e9, -1.0e9);
    for(size_t i = 0; i < objVertices.size(); i += 3) {
        glm::vec3 vtx(objVertices[i], objVertices[i + 1], objVertices[i + 2]);
        min = glm::min(min, vtx);
        max = glm::max(max, vtx);
    }
    centroid = (min + max) * 0.5f;
    radius = glm::max(glm::length(max - centroid), glm::length(min - centroid));

    for(const auto &shape : objShapes) {
        groups.emplace_back(shape.name);

        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            Face face;
            face.material = static_cast<size_t>(shape.mesh.material_ids[f] + 1);
            face.group = groups.size() - 1;
            face.vertices.reserve(3);
            face.normals.reserve(3);

            for(size_t v = 0; v < 3; ++v) {
                int vertexIndex = shape.mesh.indices[3 * f + v].vertex_index;
                if(vertexIndex >= 0) {
                    glm::vec3 vtx;
                    for(size_t w = 0; w < 3; ++w) {
                        vtx[w] = objVertices[3 * vertexIndex + w];
                    }
                    face.vertices.emplace_back(std::move(vtx));
                } else {
                    face.vertices.emplace_back(0.0, 0.0, 0.0);
                }
            }

            glm::vec3 v0 = face.vertices[face.vertices.size() - 3];
            glm::vec3 v1 = face.vertices[face.vertices.size() - 2];
            glm::vec3 v2 = face.vertices[face.vertices.size() - 1];
            face.centroid = (v0 + v1 + v2) / 3.0f;
            glm::vec3 defaultNormal = glm::cross(v1 - v0, v2 - v0);

            for(size_t v = 0; v < 3; ++v) {
                int normalIndex = shape.mesh.indices[3 * f + v].normal_index;
                if(normalIndex >= 0) {
                    glm::vec3 normal;
                    for(size_t w = 0; w < 3; ++w) {
                        normal[w] = objNormals[3 * normalIndex + w];
                    }
                    face.normals.emplace_back(std::move(normal));
                } else {
                    face.normals.emplace_back(defaultNormal);
                }
            }

            faces.emplace_back(std::move(face));

            materialToFaces[face.material].emplace_back(faces.size() - 1);
            groupToFaces[face.group].emplace_back(faces.size() - 1);

            if(materials[face.material].d < 1.0) {
                transparentFaces.emplace_back(faces.size() - 1);
            }
        }
    }
}

void Mesh::load(const std::string &path, const std::string &mtlDir) {
    materials.clear();
    groups.clear();
    faces.clear();
    materialToFaces.clear();
    groupToFaces.clear();
    transparentFaces.clear();

    loadBase(path, mtlDir);
}
