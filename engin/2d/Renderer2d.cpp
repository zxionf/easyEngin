#include "Renderer2d.hpp"
#include <iostream>
#include <cmath>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;
out vec4 Color;

void main() {
    gl_Position = projection * view * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D texture1;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = texture(texture1, TexCoord) * Color;
    } else {
        FragColor = Color;
    }
}
)";

Renderer2D::Renderer2D(const Application& appContext) 
    : appContext(appContext), shaderProgram(0), VAO(0), VBO(0), 
      currentTexture(0), viewOffsetX(0), viewOffsetY(0), viewZoom(1.0f),
      batchInProgress(false) {
    vertices.reserve(MAX_VERTICES);
}

Renderer2D::~Renderer2D() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
    }
}

// 修改 initialize 方法
bool Renderer2D::initialize() {
    if (!setupShaders()) {
        std::cerr << "Failed to setup shaders" << std::endl;
        return false;
    }
    
    setupBuffers();
    
    // 设置正确的投影矩阵
    projectionMatrix = glm::ortho(0.0f, (float)appContext.getWidth(), (float)appContext.getHeight(), 0.0f, -1.0f, 1.0f);
    
    return true;
}

void Renderer2D::beginFrame() {
    batchInProgress = true;
    vertices.clear();
}

void Renderer2D::endFrame() {
    flushBatch();
    batchInProgress = false;
}

void Renderer2D::drawSprite(const Sprite2D& sprite) {
    auto texture = sprite.getTexture();
    if (!texture || !texture->isValid()) {
        return;
    }
    
    glUseProgram(shaderProgram);
    
    // 设置投影矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, 
                      glm::value_ptr(projectionMatrix));
    
    // 设置视图矩阵
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-viewOffsetX, -viewOffsetY, 0.0f));
    view = glm::scale(view, glm::vec3(viewZoom, viewZoom, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, 
                      glm::value_ptr(view));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), GL_TRUE);
    
    texture->bind();
    
    Vector2 pos = sprite.getPosition();
    Vector2 size = sprite.getSize();
    const float* color = sprite.getColor();
    
    // 修复顶点数据 - 使用正确的纹理坐标
    float vertices[] = {
        // 位置             纹理坐标   颜色
        pos.x, pos.y,          0.0f, 0.0f, color[0], color[1], color[2], color[3],
        pos.x + size.x, pos.y, 1.0f, 0.0f, color[0], color[1], color[2], color[3],
        pos.x + size.x, pos.y + size.y, 1.0f, 1.0f, color[0], color[1], color[2], color[3],
        pos.x, pos.y + size.y, 0.0f, 1.0f, color[0], color[1], color[2], color[3]
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // 创建临时EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // 更新顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    // 更新索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // 清理
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &EBO);
}

// 在 Renderer2d.cpp 中添加
void Renderer2D::drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a) {
    flushBatch(); // 确保之前的绘制已完成
    
    glUseProgram(shaderProgram);
    
    // 设置投影矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, 
                      glm::value_ptr(projectionMatrix));
    
    // 设置视图矩阵
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-viewOffsetX, -viewOffsetY, 0.0f));
    view = glm::scale(view, glm::vec3(viewZoom, viewZoom, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, 
                      glm::value_ptr(view));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), GL_FALSE); // 不使用纹理
    
    // 直接生成矩形顶点数据
    float vertices[] = {
        // 位置             纹理坐标   颜色
        x, y,              0.0f, 0.0f, r, g, b, a,
        x + width, y,      1.0f, 0.0f, r, g, b, a,
        x + width, y + height, 1.0f, 1.0f, r, g, b, a,
        x, y + height,     0.0f, 1.0f, r, g, b, a
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // 创建临时EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // 更新顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    // 更新索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // 清理
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &EBO);
}

void Renderer2D::setViewMatrix(float offsetX, float offsetY, float zoom) {
    viewOffsetX = offsetX;
    viewOffsetY = offsetY;
    viewZoom = zoom;
}

void Renderer2D::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

bool Renderer2D::setupShaders() {
    // 编译顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // 检查顶点着色器编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // 编译片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // 检查片段着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // 链接着色器程序
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void Renderer2D::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // 预分配缓冲区大小
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    
    // 顶点属性指针
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Renderer2D::flushBatch() {
    // 批处理刷新实现
    // 在实际引擎中，这里会将所有顶点数据上传并绘制
}