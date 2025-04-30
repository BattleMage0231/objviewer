#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 target, float distance, float yaw, float pitch, float fov) :
    target {target},
    distance {distance},
    yaw {yaw},
    pitch {pitch},
    fov {fov}
{}

void Camera::move(glm::vec3 target) {
    this->target = target;
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch = glm::clamp(pitch + deltaPitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
}

void Camera::zoom(float deltaZoom) {
    distance = glm::max(0.1f, distance + deltaZoom);
}

glm::vec3 Camera::getCameraPosition() const {
    float x = distance * glm::cos(pitch) * glm::sin(yaw);
    float y = distance * glm::sin(pitch);
    float z = distance * glm::cos(pitch) * glm::cos(yaw);
    return target + glm::vec3(x, y, z);
}

glm::mat4 Camera::getProjection(float aspectRatio) const {
    return glm::perspective(fov, aspectRatio, 0.1f, 1000.0f);
}

glm::mat4 Camera::getView() const {
    glm::vec3 camPos = getCameraPosition();
    return glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}
