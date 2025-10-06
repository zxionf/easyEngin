#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera2D {
public:
    Camera2D();
    
    void setProjection(float left, float right, float bottom, float top);
    void setProjection(int screenWidth, int screenHeight);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;
    
    // 2D 相机控制
    void setPosition(const glm::vec2& position);
    void move(const glm::vec2& offset);
    void setZoom(float zoom);
    void zoom(float factor);
    void setRotation(float rotation);
    void rotate(float angle);
    
    // 坐标转换
    glm::vec2 screenToWorld(const glm::vec2& screenPos) const;
    glm::vec2 worldToScreen(const glm::vec2& worldPos) const;
    
    // 获取相机属性
    glm::vec2 getPosition() const { return position; }
    float getZoom() const { return zoomLevel; }
    float getRotation() const { return rotation; }
    glm::vec2 getViewportSize() const { return viewportSize; }
    
private:
    void updateMatrices();
    
private:
    glm::vec2 position;
    float zoomLevel;
    float rotation;
    
    glm::vec2 viewportSize;
    
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 viewProjectionMatrix;
    
    bool needsUpdate;
};