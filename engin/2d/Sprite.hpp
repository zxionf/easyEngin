#pragma once
#include "Vector2.hpp"
#include "Texture2d.hpp"
#include "Rect.hpp"
#include <memory>

class Sprite2D {
public:
    Sprite2D();
    Sprite2D(const std::shared_ptr<Texture2D>& texture);
    
    // 变换
    void setPosition(const Vector2<>& position) { this->position = position; }
    void setScale(const Vector2<>& scale) { this->scale = scale; }
    void setRotation(float rotation) { this->rotation = rotation; }
    void setColor(float r, float g, float b, float a = 1.0f) { color[0] = r; color[1] = g; color[2] = b; color[3] = a; }
    
    void move(const Vector2<>& offset) { position += offset; }
    void scaleBy(const Vector2<>& factor) { scale.x *= factor.x; scale.y *= factor.y; }
    void rotate(float angle) { rotation += angle; }
    
    // 纹理
    void setTexture(std::shared_ptr<Texture2D> texture) { this->texture = texture; }
    
    // 获取属性
    Vector2<> getPosition() const { return position; }
    Vector2<> getScale() const { return scale; }
    float getRotation() const { return rotation; }
    const float* getColor() const { return color; }
    std::shared_ptr<Texture2D> getTexture() const { return texture; }
    
    // 工具函数
    Vector2<> getSize() const;
    Rect getBoundingBox() const;
    
private:
    Vector2<> position;
    Vector2<> scale;
    float rotation;
    float color[4];
    std::shared_ptr<Texture2D> texture;
};