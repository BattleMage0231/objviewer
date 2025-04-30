#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, float yaw, float pitch, float fov, float speed, float sensitivity) : 
    position {std::move(position)}, 
    yaw {yaw}, 
    pitch {pitch}, 
    fov {fov}, 
    speed {speed}, 
    sensitivity {sensitivity} 
{
    computeVectors();
}

void Camera::computeVectors() {
    front.x = glm::cos(yaw) * glm::cos(pitch);
    front.y = glm::sin(pitch);
    front.z = glm::sin(yaw) * glm::cos(pitch);
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::moveX(float delta) {
    position += front * speed * delta;
}

void Camera::moveY(float delta) {
    position += right * speed * delta;
}

void Camera::moveMouse(float deltaX, float deltaY) {
    yaw += deltaX * sensitivity;
    pitch += deltaY * sensitivity;
    pitch = glm::clamp(pitch, glm::radians(-89.0f), glm::radians(89.0f));
    computeVectors();
}

glm::mat4 Camera::getProjection(float aspectRatio) const {
    return glm::perspective(fov, aspectRatio, 0.1f, 100.0f);
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(position, position + front, up);
}
