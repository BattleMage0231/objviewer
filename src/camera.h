#pragma once

#include <glm/glm.hpp>
#include <optional>

class Camera {
    glm::vec3 target;
    float distance;
    float yaw;
    float pitch;
    mutable std::optional<glm::vec3> position;

    void computeVectors();

public:
    Camera() {}
    Camera(glm::vec3 target, float distance, float yaw, float pitch);

    void move(glm::vec3 target);
    void rotate(float deltaYaw, float deltaPitch);
    void zoom(float deltaZoom);

    glm::vec3 getPosition() const;
    glm::mat4 getProjection(float aspectRatio) const;
    glm::mat4 getView() const;
};
