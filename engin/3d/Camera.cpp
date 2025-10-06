#include "Camera.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch),
      movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f),
      projectionMode(PERSPECTIVE) {
    
    // 初始化透视投影参数（默认）
    projection.perspective.fov = 45.0f;
    projection.perspective.aspect = 16.0f / 9.0f;
    projection.perspective.near = 0.1f;
    projection.perspective.far = 100.0f;
    
    // 初始化正交投影参数
    projection.orthographic.left = -10.0f;
    projection.orthographic.right = 10.0f;
    projection.orthographic.bottom = -10.0f;
    projection.orthographic.top = 10.0f;
    projection.orthographic.near = 0.1f;
    projection.orthographic.far = 100.0f;
    
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    if (projectionMode == PERSPECTIVE) {
        return glm::perspective(
            glm::radians(projection.perspective.fov),
            projection.perspective.aspect,
            projection.perspective.near,
            projection.perspective.far
        );
    } else {
        return glm::ortho(
            projection.orthographic.left,
            projection.orthographic.right,
            projection.orthographic.bottom,
            projection.orthographic.top,
            projection.orthographic.near,
            projection.orthographic.far
        );
    }
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
    projectionMode = PERSPECTIVE;
    projection.perspective.fov = fov;
    projection.perspective.aspect = aspect;
    projection.perspective.near = near;
    projection.perspective.far = far;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float near, float far) {
    projectionMode = ORTHOGRAPHIC;
    projection.orthographic.left = left;
    projection.orthographic.right = right;
    projection.orthographic.bottom = bottom;
    projection.orthographic.top = top;
    projection.orthographic.near = near;
    projection.orthographic.far = far;
}

void Camera::setProjectionMode(ProjectionMode mode) {
    projectionMode = mode;
}

void Camera::toggleProjectionMode() {
    projectionMode = (projectionMode == PERSPECTIVE) ? ORTHOGRAPHIC : PERSPECTIVE;
    std::cout << "Switched to " << (projectionMode == PERSPECTIVE ? "Perspective" : "Orthographic") 
              << " projection" << std::endl;
}

void Camera::processKeyboard(Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // 确保当俯仰角超出边界时屏幕不会被翻转
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    if (projectionMode == PERSPECTIVE) {
        projection.perspective.fov -= yoffset;
        if (projection.perspective.fov < 1.0f)
            projection.perspective.fov = 1.0f;
        if (projection.perspective.fov > 45.0f)
            projection.perspective.fov = 45.0f;
    } else {
        // 对于正交投影，可以用滚轮调整正交投影的大小
        float zoomFactor = 1.0f - yoffset * 0.1f;
        projection.orthographic.left *= zoomFactor;
        projection.orthographic.right *= zoomFactor;
        projection.orthographic.bottom *= zoomFactor;
        projection.orthographic.top *= zoomFactor;
    }
}

void Camera::setYawPitch(float newYaw, float newPitch) {
    yaw = newYaw;
    pitch = newPitch;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // 计算新的前向量
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    // 重新计算右向量和上向量
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}