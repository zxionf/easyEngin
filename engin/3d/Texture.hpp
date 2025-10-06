#pragma once
#include <string>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture {
public:
    enum TextureType {
        TEXTURE_FILE,       // 从文件加载
        TEXTURE_CHECKER,    // 棋盘格纹理
        TEXTURE_SOLID,      // 纯色纹理
        TEXTURE_TEST,       // 测试图案
        TEXTURE_DATA        // 从内存数据创建
    };
    
    enum PixelFormat {
        FORMAT_RGB,
        FORMAT_RGBA,
        FORMAT_GRAYSCALE
    };
    
    Texture();
    ~Texture();
    
    // 创建纹理的方法
    bool loadFromFile(const std::string& path);
    bool createCheckerboard(int width = 256, int height = 256, int checkerSize = 32);
    bool createSolidColor(int width = 256, int height = 256, 
                         const glm::vec3& color = glm::vec3(0.8f, 0.2f, 0.2f));
    bool createTestPattern(int width = 256, int height = 256);
    bool createFromData(int width, int height, const std::vector<unsigned char>& data, 
                       PixelFormat format = FORMAT_RGB);
    
    // 纹理数据操作
    bool setPixel(int x, int y, const glm::vec3& color);
    bool setPixel(int x, int y, const glm::vec4& color);
    glm::vec4 getPixel(int x, int y) const;
    
    bool fillRegion(int x, int y, int width, int height, const glm::vec3& color);
    bool drawLine(int x1, int y1, int x2, int y2, const glm::vec3& color, int thickness = 1);
    bool drawCircle(int centerX, int centerY, int radius, const glm::vec3& color, bool filled = false);
    bool drawRectangle(int x, int y, int width, int height, const glm::vec3& color, bool filled = false);
    
    // 滤镜和效果
    bool applyGrayscale();
    bool applyInvert();
    bool applyBlur(int radius = 1);
    bool applyEdgeDetection();
    bool applySepia();
    
    // 变换操作
    bool resize(int newWidth, int newHeight);
    bool rotate(float angle); // 角度制
    bool flipHorizontal();
    bool flipVertical();
    
    // 纹理操作
    void bind(GLuint unit = 0) const;
    void unbind() const;
    void updateGPU(); // 将CPU数据更新到GPU
    
    // 获取纹理信息
    GLuint getID() const { return textureID; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    TextureType getType() const { return type; }
    PixelFormat getFormat() const { return format; }
    const std::string& getFilePath() const { return filePath; }
    const std::vector<unsigned char>& getData() const { return pixelData; }
    
    // 工具函数
    bool saveToFile(const std::string& path) const;
    bool hasData() const { return !pixelData.empty(); }
    void clearData(); // 释放CPU数据以节省内存
    
private:
    bool setupTextureParameters();
    bool uploadToGPU();
    void ensureDataLoaded();
    int getBytesPerPixel() const;
    int getPixelIndex(int x, int y) const;
    bool isValidCoordinate(int x, int y) const;
    
    // Bresenham 画线算法
    void drawLineLow(int x1, int y1, int x2, int y2, const glm::vec3& color);
    void drawLineHigh(int x1, int y1, int x2, int y2, const glm::vec3& color);
    
private:
    GLuint textureID;
    int width, height;
    GLenum internalFormat;
    GLenum imageFormat;
    
    TextureType type;
    PixelFormat format;
    std::string filePath;
    
    // 纹理数据存储
    std::vector<unsigned char> pixelData;
    
    // 程序生成纹理的参数
    struct {
        int checkerSize;
        glm::vec3 solidColor;
    } generatorParams;
    
    // 标记数据是否已修改需要更新到GPU
    bool dirty;
};