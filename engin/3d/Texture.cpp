#include "Texture.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>

// 如果使用 stb_image，取消下面的注释
/*
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
*/

Texture::Texture() 
    : textureID(0), width(0), height(0), 
      internalFormat(GL_RGB), imageFormat(GL_RGB),
      type(TEXTURE_CHECKER), format(FORMAT_RGB),
      dirty(false) {
    
    // 默认生成器参数
    generatorParams.checkerSize = 32;
    generatorParams.solidColor = glm::vec3(0.8f, 0.2f, 0.2f);
}

Texture::~Texture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

bool Texture::loadFromFile(const std::string& path) {
    // 方法1: 使用 stb_image（推荐）
    /*
    stbi_set_flip_vertically_on_load(true);
    
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }
    
    // 确定格式
    if (nrChannels == 1) {
        internalFormat = GL_RED;
        imageFormat = GL_RED;
        format = FORMAT_GRAYSCALE;
    } else if (nrChannels == 3) {
        internalFormat = GL_RGB;
        imageFormat = GL_RGB;
        format = FORMAT_RGB;
    } else if (nrChannels == 4) {
        internalFormat = GL_RGBA;
        imageFormat = GL_RGBA;
        format = FORMAT_RGBA;
    }
    
    // 存储数据
    pixelData.assign(data, data + width * height * nrChannels);
    stbi_image_free(data);
    */
    
    // 方法2: 使用回退的测试纹理
    std::cerr << "stb_image not available. Using fallback texture for: " << path << std::endl;
    return createTestPattern(256, 256);
    
    type = TEXTURE_FILE;
    filePath = path;
    dirty = true;
    
    return uploadToGPU();
}

bool Texture::createCheckerboard(int width, int height, int checkerSize) {
    this->width = width;
    this->height = height;
    this->format = FORMAT_RGB;
    internalFormat = GL_RGB;
    imageFormat = GL_RGB;
    type = TEXTURE_CHECKER;
    generatorParams.checkerSize = checkerSize;
    
    pixelData.resize(width * height * 3);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            
            // 确定棋盘格颜色
            bool isWhite = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            
            if (isWhite) {
                pixelData[index + 0] = 255; // R
                pixelData[index + 1] = 255; // G
                pixelData[index + 2] = 255; // B
            } else {
                pixelData[index + 0] = 100; // R
                pixelData[index + 1] = 100; // G
                pixelData[index + 2] = 255; // B
            }
        }
    }
    
    dirty = true;
    return uploadToGPU();
}

bool Texture::createSolidColor(int width, int height, const glm::vec3& color) {
    this->width = width;
    this->height = height;
    this->format = FORMAT_RGB;
    internalFormat = GL_RGB;
    imageFormat = GL_RGB;
    type = TEXTURE_SOLID;
    generatorParams.solidColor = color;
    
    pixelData.resize(width * height * 3);
    
    unsigned char r = static_cast<unsigned char>(color.r * 255);
    unsigned char g = static_cast<unsigned char>(color.g * 255);
    unsigned char b = static_cast<unsigned char>(color.b * 255);
    
    for (int i = 0; i < width * height * 3; i += 3) {
        pixelData[i + 0] = r;
        pixelData[i + 1] = g;
        pixelData[i + 2] = b;
    }
    
    dirty = true;
    return uploadToGPU();
}

bool Texture::createTestPattern(int width, int height) {
    this->width = width;
    this->height = height;
    this->format = FORMAT_RGB;
    internalFormat = GL_RGB;
    imageFormat = GL_RGB;
    type = TEXTURE_TEST;
    
    pixelData.resize(width * height * 3);
    
    // 创建彩色测试图案
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            
            // 归一化坐标
            float nx = static_cast<float>(x) / width;
            float ny = static_cast<float>(y) / height;
            
            // 创建彩色渐变
            pixelData[index + 0] = static_cast<unsigned char>(nx * 255);        // R
            pixelData[index + 1] = static_cast<unsigned char>(ny * 255);        // G
            pixelData[index + 2] = static_cast<unsigned char>((1.0f - nx) * 255); // B
            
            // 添加一些图案
            float dist = std::sqrt((nx - 0.5f) * (nx - 0.5f) + (ny - 0.5f) * (ny - 0.5f));
            if (dist < 0.3f && std::fmod(dist * 20.0f, 1.0f) > 0.5f) {
                pixelData[index + 0] = 255 - pixelData[index + 0];
                pixelData[index + 1] = 255 - pixelData[index + 1];
                pixelData[index + 2] = 255 - pixelData[index + 2];
            }
        }
    }
    
    dirty = true;
    return uploadToGPU();
}

bool Texture::createFromData(int width, int height, const std::vector<unsigned char>& data, PixelFormat format) {
    this->width = width;
    this->height = height;
    this->format = format;
    this->type = TEXTURE_DATA;
    
    // 设置OpenGL格式
    switch (format) {
        case FORMAT_RGB:
            internalFormat = GL_RGB;
            imageFormat = GL_RGB;
            break;
        case FORMAT_RGBA:
            internalFormat = GL_RGBA;
            imageFormat = GL_RGBA;
            break;
        case FORMAT_GRAYSCALE:
            internalFormat = GL_RED;
            imageFormat = GL_RED;
            break;
    }
    
    pixelData = data;
    dirty = true;
    return uploadToGPU();
}

// 像素操作
bool Texture::setPixel(int x, int y, const glm::vec3& color) {
    if (!isValidCoordinate(x, y)) return false;
    ensureDataLoaded();
    
    int index = getPixelIndex(x, y);
    int bpp = getBytesPerPixel();
    
    pixelData[index + 0] = static_cast<unsigned char>(color.r * 255);
    pixelData[index + 1] = static_cast<unsigned char>(color.g * 255);
    pixelData[index + 2] = static_cast<unsigned char>(color.b * 255);
    
    if (bpp == 4 && format != FORMAT_GRAYSCALE) {
        pixelData[index + 3] = 255; // 不透明
    }
    
    dirty = true;
    return true;
}

bool Texture::setPixel(int x, int y, const glm::vec4& color) {
    if (!isValidCoordinate(x, y)) return false;
    ensureDataLoaded();
    
    int index = getPixelIndex(x, y);
    int bpp = getBytesPerPixel();
    
    pixelData[index + 0] = static_cast<unsigned char>(color.r * 255);
    pixelData[index + 1] = static_cast<unsigned char>(color.g * 255);
    pixelData[index + 2] = static_cast<unsigned char>(color.b * 255);
    
    if (bpp == 4 && format != FORMAT_GRAYSCALE) {
        pixelData[index + 3] = static_cast<unsigned char>(color.a * 255);
    }
    
    dirty = true;
    return true;
}

glm::vec4 Texture::getPixel(int x, int y) const {
    if (!isValidCoordinate(x, y) || pixelData.empty()) {
        return glm::vec4(0.0f);
    }
    
    int index = getPixelIndex(x, y);
    int bpp = getBytesPerPixel();
    
    glm::vec4 color(0.0f);
    color.r = pixelData[index + 0] / 255.0f;
    color.g = pixelData[index + 1] / 255.0f;
    color.b = pixelData[index + 2] / 255.0f;
    
    if (bpp == 4 && format != FORMAT_GRAYSCALE) {
        color.a = pixelData[index + 3] / 255.0f;
    } else {
        color.a = 1.0f;
    }
    
    return color;
}

bool Texture::fillRegion(int x, int y, int regionWidth, int regionHeight, const glm::vec3& color) {
    ensureDataLoaded();
    
    for (int dy = 0; dy < regionHeight; ++dy) {
        for (int dx = 0; dx < regionWidth; ++dx) {
            int px = x + dx;
            int py = y + dy;
            
            if (isValidCoordinate(px, py)) {
                setPixel(px, py, color);
            }
        }
    }
    
    return true;
}

// 绘图操作
bool Texture::drawLine(int x1, int y1, int x2, int y2, const glm::vec3& color, int thickness) {
    ensureDataLoaded();
    
    // 简单的 Bresenham 算法实现
    if (std::abs(y2 - y1) < std::abs(x2 - x1)) {
        if (x1 > x2) {
            drawLineLow(x2, y2, x1, y1, color);
        } else {
            drawLineLow(x1, y1, x2, y2, color);
        }
    } else {
        if (y1 > y2) {
            drawLineHigh(x2, y2, x1, y1, color);
        } else {
            drawLineHigh(x1, y1, x2, y2, color);
        }
    }
    
    // 简单的厚度实现
    if (thickness > 1) {
        for (int t = 1; t < thickness; ++t) {
            drawLine(x1, y1 + t, x2, y2 + t, color, 1);
            drawLine(x1 + t, y1, x2 + t, y2, color, 1);
        }
    }
    
    return true;
}

bool Texture::drawCircle(int centerX, int centerY, int radius, const glm::vec3& color, bool filled) {
    ensureDataLoaded();
    
    if (filled) {
        // 填充圆
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                if (x * x + y * y <= radius * radius) {
                    setPixel(centerX + x, centerY + y, color);
                }
            }
        }
    } else {
        // 空心圆
        int x = radius;
        int y = 0;
        int err = 0;
        
        while (x >= y) {
            setPixel(centerX + x, centerY + y, color);
            setPixel(centerX + y, centerY + x, color);
            setPixel(centerX - y, centerY + x, color);
            setPixel(centerX - x, centerY + y, color);
            setPixel(centerX - x, centerY - y, color);
            setPixel(centerX - y, centerY - x, color);
            setPixel(centerX + y, centerY - x, color);
            setPixel(centerX + x, centerY - y, color);
            
            if (err <= 0) {
                y += 1;
                err += 2 * y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2 * x + 1;
            }
        }
    }
    
    return true;
}

bool Texture::drawRectangle(int x, int y, int rectWidth, int rectHeight, const glm::vec3& color, bool filled) {
    ensureDataLoaded();
    
    if (filled) {
        fillRegion(x, y, rectWidth, rectHeight, color);
    } else {
        // 画四条边
        drawLine(x, y, x + rectWidth, y, color);
        drawLine(x + rectWidth, y, x + rectWidth, y + rectHeight, color);
        drawLine(x + rectWidth, y + rectHeight, x, y + rectHeight, color);
        drawLine(x, y + rectHeight, x, y, color);
    }
    
    return true;
}

// 滤镜效果
bool Texture::applyGrayscale() {
    ensureDataLoaded();
    
    for (int i = 0; i < pixelData.size(); i += getBytesPerPixel()) {
        unsigned char gray = static_cast<unsigned char>(
            0.299f * pixelData[i + 0] + 
            0.587f * pixelData[i + 1] + 
            0.114f * pixelData[i + 2]
        );
        
        pixelData[i + 0] = gray;
        pixelData[i + 1] = gray;
        pixelData[i + 2] = gray;
    }
    
    dirty = true;
    return true;
}

bool Texture::applyInvert() {
    ensureDataLoaded();
    
    for (int i = 0; i < pixelData.size(); i += getBytesPerPixel()) {
        pixelData[i + 0] = 255 - pixelData[i + 0];
        pixelData[i + 1] = 255 - pixelData[i + 1];
        pixelData[i + 2] = 255 - pixelData[i + 2];
    }
    
    dirty = true;
    return true;
}

bool Texture::applySepia() {
    ensureDataLoaded();
    
    for (int i = 0; i < pixelData.size(); i += getBytesPerPixel()) {
        unsigned char r = pixelData[i + 0];
        unsigned char g = pixelData[i + 1];
        unsigned char b = pixelData[i + 2];
        
        pixelData[i + 0] = std::min(255, static_cast<int>(r * 0.393 + g * 0.769 + b * 0.189));
        pixelData[i + 1] = std::min(255, static_cast<int>(r * 0.349 + g * 0.686 + b * 0.168));
        pixelData[i + 2] = std::min(255, static_cast<int>(r * 0.272 + g * 0.534 + b * 0.131));
    }
    
    dirty = true;
    return true;
}

// 纹理操作
void Texture::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::updateGPU() {
    if (dirty && !pixelData.empty()) {
        uploadToGPU();
        dirty = false;
    }
}

// 工具函数
bool Texture::saveToFile(const std::string& path) const {
    if (pixelData.empty()) return false;
    
    // 使用 stb_image_write 保存图片
    /*
    int result = 0;
    if (format == FORMAT_RGB) {
        result = stbi_write_png(path.c_str(), width, height, 3, pixelData.data(), width * 3);
    } else if (format == FORMAT_RGBA) {
        result = stbi_write_png(path.c_str(), width, height, 4, pixelData.data(), width * 4);
    }
    
    return result != 0;
    */
    
    std::cerr << "stb_image_write not available. Cannot save texture to: " << path << std::endl;
    return false;
}

void Texture::clearData() {
    pixelData.clear();
    pixelData.shrink_to_fit();
}

// 私有辅助函数
bool Texture::setupTextureParameters() {
    if (textureID == 0) {
        glGenTextures(1, &textureID);
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return true;
}

bool Texture::uploadToGPU() {
    if (pixelData.empty()) return false;
    
    setupTextureParameters();
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, pixelData.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture::ensureDataLoaded() {
    if (pixelData.empty() && textureID != 0) {
        // 从GPU读取数据（性能开销较大，慎用）
        pixelData.resize(width * height * getBytesPerPixel());
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glGetTexImage(GL_TEXTURE_2D, 0, imageFormat, GL_UNSIGNED_BYTE, pixelData.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

int Texture::getBytesPerPixel() const {
    switch (format) {
        case FORMAT_RGB: return 3;
        case FORMAT_RGBA: return 4;
        case FORMAT_GRAYSCALE: return 1;
        default: return 3;
    }
}

int Texture::getPixelIndex(int x, int y) const {
    return (y * width + x) * getBytesPerPixel();
}

bool Texture::isValidCoordinate(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// Bresenham 算法实现
void Texture::drawLineLow(int x1, int y1, int x2, int y2, const glm::vec3& color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    int D = 2 * dy - dx;
    int y = y1;
    
    for (int x = x1; x <= x2; ++x) {
        setPixel(x, y, color);
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void Texture::drawLineHigh(int x1, int y1, int x2, int y2, const glm::vec3& color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    int D = 2 * dx - dy;
    int x = x1;
    
    for (int y = y1; y <= y2; ++y) {
        setPixel(x, y, color);
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }
        D += 2 * dx;
    }
}