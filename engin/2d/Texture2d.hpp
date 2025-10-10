#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>

class Texture2D {
public:
    Texture2D();
    Texture2D(const std::string& filePath);
    ~Texture2D();
    
    // ��������
    bool createFromFile(const std::string& filename);
    bool createSolidColor(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    bool createCheckerboard(int width, int height, int checkerSize, 
                           unsigned char color1r, unsigned char color1g, unsigned char color1b,
                           unsigned char color2r, unsigned char color2g, unsigned char color2b);
    
    // �������
    void bind() const;
    void unbind() const;
    
    // ��ȡ��Ϣ
    GLuint getID() const { return textureID; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isValid() const { return textureID != 0; }
    
private:
    bool uploadToGPU(const unsigned char* data, int width, int height, GLenum format, GLenum internalFormat);
    
    GLuint textureID;
    int width, height;
};