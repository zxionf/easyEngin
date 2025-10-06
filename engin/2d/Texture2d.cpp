#include "Texture2d.hpp"
#include <iostream>
#include <vector>

Texture2D::Texture2D() : textureID(0), width(0), height(0) {}

Texture2D::~Texture2D() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

bool Texture2D::createFromFile(const std::string& filename) {
    // 这里应该使用stb_image或其他图像加载库
    // 为了简单，我们创建一个回退纹理
    std::cerr << "Image loading not implemented. Creating fallback texture for: " << filename << std::endl;
    return createCheckerboard(128, 128, 16, 255, 255, 255, 100, 100, 255);
}

bool Texture2D::createSolidColor(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    this->width = width;
    this->height = height;
    
    std::vector<unsigned char> data(width * height * 4);
    for (int i = 0; i < width * height * 4; i += 4) {
        data[i + 0] = r;
        data[i + 1] = g;
        data[i + 2] = b;
        data[i + 3] = a;
    }
    
    return uploadToGPU(data, GL_RGBA);
}

bool Texture2D::createCheckerboard(int width, int height, int checkerSize, 
                                  unsigned char color1r, unsigned char color1g, unsigned char color1b,
                                  unsigned char color2r, unsigned char color2g, unsigned char color2b) {
    this->width = width;
    this->height = height;
    
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            bool isColor1 = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            
            if (isColor1) {
                data[index + 0] = color1r;
                data[index + 1] = color1g;
                data[index + 2] = color1b;
            } else {
                data[index + 0] = color2r;
                data[index + 1] = color2g;
                data[index + 2] = color2b;
            }
        }
    }
    
    return uploadToGPU(data, GL_RGB);
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::uploadToGPU(const std::vector<unsigned char>& data, GLenum format) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}