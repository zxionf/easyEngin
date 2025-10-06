#include "Transform.hpp"

Transform::Transform() 
    : position(0.0f, 0.0f, 0.0f), 
      rotation(0.0f, 0.0f, 0.0f), 
      scale(1.0f, 1.0f, 1.0f) {
}

void Transform::setPosition(const glm::vec3& position) {
    this->position = position;
}

void Transform::setRotation(const glm::vec3& rotation) {
    this->rotation = rotation;
}

void Transform::setScale(const glm::vec3& scale) {
    this->scale = scale;
}

void Transform::translate(const glm::vec3& translation) {
    this->position += translation;
}

void Transform::rotate(float angle, const glm::vec3& axis) {
    // 简化实现，实际应用中需要更复杂的旋转处理
    this->rotation += axis * angle;
}

void Transform::scaleBy(const glm::vec3& scaling) {
    this->scale *= scaling;
}

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}