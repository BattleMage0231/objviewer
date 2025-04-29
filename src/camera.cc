#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 dir) : position {std::move(position)}, dir {glm::normalize(dir)} {
    computeVectors();
}

void Camera::computeVectors() {
    right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir));
    up = glm::cross(dir, right);
}

// temporary
void Camera::update(float angle) {
    position.x = 1 + 8 * glm::cos(angle);
    position.y = 1 + 8 * glm::sin(angle);
    position.z = 2.0;
    computeVectors();
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(position, position + dir, up);
}
