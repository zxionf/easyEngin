#pragma once
#include <string>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture {
public:
    enum TextureType {
        TEXTURE_FILE,       // ���ļ�����
        TEXTURE_CHECKER,    // ���̸�����
        TEXTURE_SOLID,      // ��ɫ����
        TEXTURE_TEST,       // ����ͼ��
        TEXTURE_DATA        // ���ڴ����ݴ���
    };
    
    enum PixelFormat {
        FORMAT_RGB,
        FORMAT_RGBA,
        FORMAT_GRAYSCALE
    };
    
    Texture();
    ~Texture();
    
    // ��������ķ���
    bool loadFromFile(const std::string& path);
    bool createCheckerboard(int width = 256, int height = 256, int checkerSize = 32);
    bool createSolidColor(int width = 256, int height = 256, 
                         const glm::vec3& color = glm::vec3(0.8f, 0.2f, 0.2f));
    bool createTestPattern(int width = 256, int height = 256);
    bool createFromData(int width, int height, const std::vector<unsigned char>& data, 
                       PixelFormat format = FORMAT_RGB);
    
    // �������ݲ���
    bool setPixel(int x, int y, const glm::vec3& color);
    bool setPixel(int x, int y, const glm::vec4& color);
    glm::vec4 getPixel(int x, int y) const;
    
    bool fillRegion(int x, int y, int width, int height, const glm::vec3& color);
    bool drawLine(int x1, int y1, int x2, int y2, const glm::vec3& color, int thickness = 1);
    bool drawCircle(int centerX, int centerY, int radius, const glm::vec3& color, bool filled = false);
    bool drawRectangle(int x, int y, int width, int height, const glm::vec3& color, bool filled = false);
    
    // �˾���Ч��
    bool applyGrayscale();
    bool applyInvert();
    bool applyBlur(int radius = 1);
    bool applyEdgeDetection();
    bool applySepia();
    
    // �任����
    bool resize(int newWidth, int newHeight);
    bool rotate(float angle); // �Ƕ���
    bool flipHorizontal();
    bool flipVertical();
    
    // �������
    void bind(GLuint unit = 0) const;
    void unbind() const;
    void updateGPU(); // ��CPU���ݸ��µ�GPU
    
    // ��ȡ������Ϣ
    GLuint getID() const { return textureID; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    TextureType getType() const { return type; }
    PixelFormat getFormat() const { return format; }
    const std::string& getFilePath() const { return filePath; }
    const std::vector<unsigned char>& getData() const { return pixelData; }
    
    // ���ߺ���
    bool saveToFile(const std::string& path) const;
    bool hasData() const { return !pixelData.empty(); }
    void clearData(); // �ͷ�CPU�����Խ�ʡ�ڴ�
    
private:
    bool setupTextureParameters();
    bool uploadToGPU();
    void ensureDataLoaded();
    int getBytesPerPixel() const;
    int getPixelIndex(int x, int y) const;
    bool isValidCoordinate(int x, int y) const;
    
    // Bresenham �����㷨
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
    
    // �������ݴ洢
    std::vector<unsigned char> pixelData;
    
    // ������������Ĳ���
    struct {
        int checkerSize;
        glm::vec3 solidColor;
    } generatorParams;
    
    // ��������Ƿ����޸���Ҫ���µ�GPU
    bool dirty;
};