#pragma once
#include <memory>
#include <string>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Application {
public:
    using RenderCallback = std::function<void(float)>;
    
    Application(const std::string& title = "null", int width = 800, int height = 600);
    ~Application();
    
    void run();
    void shutdown();
    
    void setRenderCallback(RenderCallback callback) { renderCallback = callback; }
    
    // 帧率控制
    void setTargetFPS(unsigned int fps);
    unsigned int getCurrentFPS() const { return currentFPS; }
    unsigned int getTargetFPS() const { return targetFPS; }
    
    // 窗口信息
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool shouldClose() const;
    
    // 输入信息
    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    void getMousePosition(double& x, double& y) const;
    
    // 时间信息
    float getDeltaTime() const { return deltaTime; }
    float getTime() const { return currentTime; }
    
private:
    bool initializeGL();
    void processInput();
    void updateTime();
    void limitFrameRate();           // 帧率限制
    void updateFrameStats();         // 帧率统计
    
    GLFWwindow* window;
    std::string title;
    int width, height;
    
    // 回调函数
    RenderCallback renderCallback;
    
    // 时间信息
    double lastTime;
    double currentTime;
    double deltaTime;
    
    // 帧率控制
    unsigned int targetFPS = 0;
    unsigned int currentFPS;
    unsigned int frameCount;
    double fpsUpdateTime;
    double frameStartTime;
    
    // 输入信息
    std::vector<bool> keys;
    std::vector<bool> mouseButtons;
    double mouseX; double mouseY;
};