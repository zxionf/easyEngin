#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform {
public:
    Transform();
    
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    
    void translate(const glm::vec3& translation);
    void rotate(float angle, const glm::vec3& axis);
    void scaleBy(const glm::vec3& scaling);
    
    glm::mat4 getModelMatrix() const;
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }
    
private:
    glm::vec3 position;
    glm::vec3 rotation; // Å·À­½Ç (pitch, yaw, roll)
    glm::vec3 scale;
};