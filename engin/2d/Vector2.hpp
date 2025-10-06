#pragma once
#include <cmath>

template<typename T = float>
class Vector2 {
public:
    T x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}

    // 操作
    void set(T x, T y){this->x = x; this->y = y;}
    void set(Vector2<T> v){this->x = v.x, this.y = v.y;}
    
    // 基本运算
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(T scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(T scalar) const { return Vector2(x / scalar, y / scalar); }
    
    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }

    bool operator==(const Vector2& other) { return x == other.x && y == other.y;}
    
    // 向量运算
    T length() const { return std::sqrt(x * x + y * y); }
    T lengthSquared() const { return x * x + y * y; }
    Vector2 normalized() const { T len = length(); return len > 0 ? *this / len : Vector2(); }
    T dot(const Vector2& other) const { return x * other.x + y * other.y; }
    T cross(const Vector2& other) const { return x * other.y - y * other.x; }
    
    // 工具函数
    static Vector2 zero() { return Vector2(0, 0); }
    static Vector2 one() { return Vector2(1, 1); }
    static Vector2 up() { return Vector2(0, 1); }
    static Vector2 down() { return Vector2(0, -1); }
    static Vector2 left() { return Vector2(-1, 0); }
    static Vector2 right() { return Vector2(1, 0); }
    
    static T distance(const Vector2& a, const Vector2& b) { return (a - b).length(); }
    static T distanceSquared(const Vector2& a, const Vector2& b) { return (a - b).lengthSquared(); }
    static Vector2 lerp(const Vector2& a, const Vector2& b, T t) { return a + (b - a) * t; }
};