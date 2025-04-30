#pragma once

#include <glm/glm.hpp>

class Camera {
    glm::vec3 target;
    float distance;
    float yaw;
    float pitch;
    float fov;

    void computeVectors();

public:
    Camera() {}
    Camera(glm::vec3 target, float distance, float yaw, float pitch, float fov);

    void move(glm::vec3 target);
    void rotate(float deltaYaw, float deltaPitch);
    void zoom(float deltaZoom);

    glm::vec3 getCameraPosition() const;
    glm::mat4 getProjection(float aspectRatio) const;
    glm::mat4 getView() const;
};
