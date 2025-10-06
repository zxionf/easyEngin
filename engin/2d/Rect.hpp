#pragma once
#include "Vector2.hpp"

class Rect {
public:
    float x, y, width, height;
    
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
    Rect(const Vector2<>& position, const Vector2<>& size) : x(position.x), y(position.y), width(size.x), height(size.y) {}
    
    // 属性访问
    Vector2<> getPosition() const { return Vector2<>(x, y); }
    Vector2<> getSize() const { return Vector2<>(width, height); }
    Vector2<> getCenter() const { return Vector2<>(x + width * 0.5f, y + height * 0.5f); }
    Vector2<> getMin() const { return Vector2<>(x, y); }
    Vector2<> getMax() const { return Vector2<>(x + width, y + height); }
    
    // 边界检查
    bool contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
    
    bool contains(const Vector2<>& point) const {
        return contains(point.x, point.y);
    }
    
    bool intersects(const Rect& other) const {
        return x < other.x + other.width && x + width > other.x &&
               y < other.y + other.height && y + height > other.y;
    }
    
    // 变换
    Rect translated(const Vector2<>& offset) const {
        return Rect(x + offset.x, y + offset.y, width, height);
    }
    
    Rect scaled(float scale) const {
        Vector2<> center = getCenter();
        Vector2<> newSize = getSize() * scale;
        return Rect(center.x - newSize.x * 0.5f, center.y - newSize.y * 0.5f, newSize.x, newSize.y);
    }
    
    // 工具函数
    static Rect fromCenter(const Vector2<>& center, const Vector2<>& size) {
        return Rect(center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y);
    }
};