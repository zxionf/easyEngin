#include "Texture2d.hpp"
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_STATIC
#include "../../deps/stb_image.h"

Texture2D::Texture2D() : textureID(0), width(0), height(0) {}
Texture2D::Texture2D(const std::string& filePath) : textureID(0), width(0), height(0) {
    createFromFile(filePath);
}

Texture2D::~Texture2D() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

bool Texture2D::createFromFile(const std::string& filename) {
    // 使用 stb_image 加载图像
    stbi_set_flip_vertically_on_load(true); // OpenGL 的纹理坐标原点在左下角，所以需要翻转
    
    int nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        std::cerr << "Error: " << stbi_failure_reason() << std::endl;
        
        // 加载失败时创建回退纹理
        std::cerr << "Creating fallback checkerboard texture instead." << std::endl;
        return createCheckerboard(128, 128, 16, 255, 255, 255, 100, 100, 255);
    }
    
    std::cout << "Loaded texture: " << filename 
              << " (" << width << "x" << height 
              << ", channels: " << nrChannels << ")" << std::endl;
    
    // 根据通道数确定格式
    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;
    
    switch (nrChannels) {
        case 1:
            format = GL_RED;
            internalFormat = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
            stbi_image_free(data);
            return false;
    }
    
    // 上传到 GPU
    bool success = uploadToGPU(data, width, height, format, internalFormat);
    
    // 释放图像数据
    stbi_image_free(data);
    
    if (!success) {
        std::cerr << "Failed to upload texture to GPU: " << filename << std::endl;
        return false;
    }
    
    std::cout << "Texture uploaded successfully: " << filename << std::endl;
    return true;
}

bool Texture2D::createSolidColor(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    std::vector<unsigned char> data(width * height * 4);
    for (int i = 0; i < width * height * 4; i += 4) {
        data[i + 0] = r;
        data[i + 1] = g;
        data[i + 2] = b;
        data[i + 3] = a;
    }
    
    return uploadToGPU(data.data(), width, height, GL_RGBA, GL_RGBA);
}

bool Texture2D::createCheckerboard(int width, int height, int checkerSize, 
                                  unsigned char color1r, unsigned char color1g, unsigned char color1b,
                                  unsigned char color2r, unsigned char color2g, unsigned char color2b) {
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
    
    return uploadToGPU(data.data(), width, height, GL_RGB, GL_RGB);
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::uploadToGPU(const unsigned char* data, int width, int height, 
                           GLenum format, GLenum internalFormat) {
    this->width = width;
    this->height = height;
    
    // 生成纹理
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // 检查 OpenGL 错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error in uploadToGPU: " << error << std::endl;
        return false;
    }
    
    return true;
}