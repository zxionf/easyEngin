#pragma once
#include "../Application.hpp"
#include "Vector2.hpp"
#include "Sprite.hpp"
#include "Texture2d.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer2D {
public:
    Renderer2D(const Application& appContext);
    ~Renderer2D();
    
    bool initialize();
    void beginFrame();
    void endFrame();
    
    // 绘制函数
    void drawSprite(const Sprite2D& sprite);
    void drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f);
    void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a = 1.0f, float thickness = 1.0f);
    void drawCircle(float x, float y, float radius, float r, float g, float b, float a = 1.0f);
    void drawText(const std::string& text, float x, float y, float scale = 1.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    
    // 状态设置
    void setViewMatrix(float offsetX, float offsetY, float zoom = 1.0f);
    void setClearColor(float r, float g, float b, float a = 1.0f);
    
private:
    const Application& appContext;


    struct Vertex {
        float position[2];
        float texCoord[2];
        float color[4];
    };
    
    bool setupShaders();
    void setupBuffers();
    void flushBatch();
    
    GLuint shaderProgram;
    GLuint VAO, VBO;
    
    std::vector<Vertex> vertices;
    GLuint currentTexture;
    
    // 视图矩阵
    float viewOffsetX, viewOffsetY;
    float viewZoom;

    glm::mat4 projectionMatrix;
    
    // 批处理状态
    bool batchInProgress;
    static const int MAX_VERTICES = 6000; // 1000个四边形
};