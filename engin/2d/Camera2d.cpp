#include "Camera2D.hpp"
#include <iostream>

Camera2D::Camera2D()
    : position(0.0f, 0.0f), zoomLevel(1.0f), rotation(0.0f),
      viewportSize(800.0f, 600.0f), needsUpdate(true) {
    setProjection(0.0f, viewportSize.x, 0.0f, viewportSize.y);
}

void Camera2D::setProjection(float left, float right, float bottom, float top) {
    projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    viewportSize = glm::vec2(right - left, top - bottom);
    needsUpdate = true;
}

void Camera2D::setProjection(int screenWidth, int screenHeight) {
    // 设置像素完美的 2D 投影
    setProjection(0.0f, static_cast<float>(screenWidth), 
                  static_cast<float>(screenHeight), 0.0f);
}

glm::mat4 Camera2D::getViewMatrix() const {
    if (needsUpdate) {
        const_cast<Camera2D*>(this)->updateMatrices();
    }
    return viewMatrix;
}

glm::mat4 Camera2D::getProjectionMatrix() const {
    return projectionMatrix;
}

glm::mat4 Camera2D::getViewProjectionMatrix() const {
    if (needsUpdate) {
        const_cast<Camera2D*>(this)->updateMatrices();
    }
    return viewProjectionMatrix;
}

void Camera2D::setPosition(const glm::vec2& position) {
    this->position = position;
    needsUpdate = true;
}

void Camera2D::move(const glm::vec2& offset) {
    position += offset;
    needsUpdate = true;
}

void Camera2D::setZoom(float zoom) {
    zoomLevel = zoom;
    needsUpdate = true;
}

void Camera2D::zoom(float factor) {
    zoomLevel *= factor;
    needsUpdate = true;
}

void Camera2D::setRotation(float rotation) {
    this->rotation = rotation;
    needsUpdate = true;
}

void Camera2D::rotate(float angle) {
    rotation += angle;
    needsUpdate = true;
}

glm::vec2 Camera2D::screenToWorld(const glm::vec2& screenPos) const {
    glm::vec4 worldPos(screenPos.x, screenPos.y, 0.0f, 1.0f);
    glm::mat4 inverseVP = glm::inverse(getViewProjectionMatrix());
    worldPos = inverseVP * worldPos;
    return glm::vec2(worldPos.x, worldPos.y);
}

glm::vec2 Camera2D::worldToScreen(const glm::vec2& worldPos) const {
    glm::vec4 screenPos(worldPos.x, worldPos.y, 0.0f, 1.0f);
    screenPos = getViewProjectionMatrix() * screenPos;
    return glm::vec2(screenPos.x, screenPos.y);
}

void Camera2D::updateMatrices() {
    // 计算视图矩阵
    viewMatrix = glm::mat4(1.0f);
    
    // 应用变换：先缩放，再旋转，最后平移
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-position, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    viewMatrix = glm::scale(viewMatrix, glm::vec3(zoomLevel, zoomLevel, 1.0f));
    
    // 计算视图投影矩阵
    viewProjectionMatrix = projectionMatrix * viewMatrix;
    
    needsUpdate = false;
}