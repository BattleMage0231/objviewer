#pragma once

#include <glm/glm.hpp>

class Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    float yaw;
    float pitch;

    float fov;
    float speed;
    float sensitivity;

    void computeVectors();

public:
    Camera() {}
    Camera(glm::vec3 position, float yaw, float pitch, float fov, float speed, float sensitivity);

    void moveX(float delta);
    void moveY(float delta);
    void moveMouse(float deltaX, float deltaY);
    glm::mat4 getProjection(float aspectRatio) const;
    glm::mat4 getView() const;
};

