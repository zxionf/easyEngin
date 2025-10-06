#include "Sprite.hpp"
#include <cmath>

Sprite2D::Sprite2D() 
    : position(0, 0), scale(1, 1), rotation(0) {
    color[0] = 1.0f; // r
    color[1] = 1.0f; // g  
    color[2] = 1.0f; // b
    color[3] = 1.0f; // a
}

Vector2<> Sprite2D::getSize() const {
    if (texture && texture->isValid()) {
        return Vector2<>(
            static_cast<float>(texture->getWidth()) * scale.x,
            static_cast<float>(texture->getHeight()) * scale.y
        );
    }
    return Vector2<>(32.0f, 32.0f); // Ĭ�ϴ�С
}

Rect Sprite2D::getBoundingBox() const {
    Vector2<> size = getSize();
    
    if (rotation == 0.0f) {
        // ����תʱ�ļ򵥱߽��
        return Rect(position.x, position.y, size.x, size.y);
    } else {
        // ����תʱ�ı߽����㣨�򻯵������߽��
        // ��ʵ�������У�����Ӧ�ü�����ת���ʵ�ʱ߽�
        Vector2<> halfSize = size * 0.5f;
        Vector2<> center = position + halfSize;
        
        // ������ת����ĸ��ǵ�
        float cosA = std::cos(rotation * 3.14159f / 180.0f);
        float sinA = std::sin(rotation * 3.14159f / 180.0f);
        
        Vector2<> corners[4] = {
            Vector2<>(-halfSize.x, -halfSize.y),
            Vector2<>( halfSize.x, -halfSize.y),
            Vector2<>( halfSize.x,  halfSize.y),
            Vector2<>(-halfSize.x,  halfSize.y)
        };
        
        // ��ת�ǵ�
        for (int i = 0; i < 4; ++i) {
            corners[i] = Vector2<>(
                corners[i].x * cosA - corners[i].y * sinA,
                corners[i].x * sinA + corners[i].y * cosA
            ) + center;
        }
        
        // ���������߽��
        float minX = corners[0].x, maxX = corners[0].x;
        float minY = corners[0].y, maxY = corners[0].y;
        
        for (int i = 1; i < 4; ++i) {
            if (corners[i].x < minX) minX = corners[i].x;
            if (corners[i].x > maxX) maxX = corners[i].x;
            if (corners[i].y < minY) minY = corners[i].y;
            if (corners[i].y > maxY) maxY = corners[i].y;
        }
        
        return Rect(minX, minY, maxX - minX, maxY - minY);
    }
}