#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    enum Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };
    
    enum ProjectionMode {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };
    
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    // 投影设置方法
    void setPerspective(float fov, float aspect, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far);
    void setProjectionMode(ProjectionMode mode);
    void toggleProjectionMode();
    
    // 输入处理
    void processKeyboard(Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    
    // 获取相机属性
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    
    // 获取投影参数
    ProjectionMode getProjectionMode() const { return projectionMode; }
    float getFOV() const { return projection.perspective.fov; }
    float getAspect() const { return projection.perspective.aspect; }
    
    // 相机控制
    void setPosition(const glm::vec3& newPosition) { position = newPosition; }
    void setYawPitch(float newYaw, float newPitch);
    
private:
    void updateCameraVectors();
    
private:
    // 相机属性
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // 欧拉角
    float yaw;
    float pitch;
    
    // 相机选项
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    // 投影系统
    ProjectionMode projectionMode;
    
    struct Perspective {
        float fov;
        float aspect;
        float near;
        float far;
    };
    
    struct Orthographic {
        float left;
        float right;
        float bottom;
        float top;
        float near;
        float far;
    };
    
    union {
        Perspective perspective;
        Orthographic orthographic;
    } projection;
};