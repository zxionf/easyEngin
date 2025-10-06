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
    
    // ͶӰ���÷���
    void setPerspective(float fov, float aspect, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far);
    void setProjectionMode(ProjectionMode mode);
    void toggleProjectionMode();
    
    // ���봦��
    void processKeyboard(Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    
    // ��ȡ�������
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    
    // ��ȡͶӰ����
    ProjectionMode getProjectionMode() const { return projectionMode; }
    float getFOV() const { return projection.perspective.fov; }
    float getAspect() const { return projection.perspective.aspect; }
    
    // �������
    void setPosition(const glm::vec3& newPosition) { position = newPosition; }
    void setYawPitch(float newYaw, float newPitch);
    
private:
    void updateCameraVectors();
    
private:
    // �������
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // ŷ����
    float yaw;
    float pitch;
    
    // ���ѡ��
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    // ͶӰϵͳ
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