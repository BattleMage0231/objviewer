#pragma once

#include <glm/glm.hpp>

class Camera {
    glm::vec3 position;
    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;

    void computeVectors();

public:
    Camera() {}
    Camera(glm::vec3 position, glm::vec3 dir);

    void update(float angle);
    glm::mat4 getView() const;
};

