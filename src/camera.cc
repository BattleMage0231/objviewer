#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 target, float distance, float yaw, float pitch) :
    target {std::move(target)},
    distance {distance},
    yaw {yaw},
    pitch {pitch}
{}

void Camera::move(glm::vec3 target) {
    this->target = target;
    position = std::nullopt;
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch = glm::clamp(pitch + deltaPitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
    position = std::nullopt;
}

void Camera::zoom(float deltaZoom) {
    distance = glm::max(0.1f, distance + deltaZoom);
    position = std::nullopt;
}

glm::vec3 Camera::getPosition() const {
    if(position) return *position;
    float x = distance * glm::cos(pitch) * glm::sin(yaw);
    float y = distance * glm::sin(pitch);
    float z = distance * glm::cos(pitch) * glm::cos(yaw);
    position = target + glm::vec3(x, y, z);
    return *position;
}

glm::mat4 Camera::getView() const {
    glm::vec3 camPos = getPosition();
    return glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}
