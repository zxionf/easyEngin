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
    
    // ֡�ʿ���
    void setTargetFPS(unsigned int fps);
    unsigned int getCurrentFPS() const { return currentFPS; }
    unsigned int getTargetFPS() const { return targetFPS; }
    
    // ������Ϣ
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool shouldClose() const;
    
    // ������Ϣ
    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    void getMousePosition(double& x, double& y) const;
    
    // ʱ����Ϣ
    float getDeltaTime() const { return deltaTime; }
    float getTime() const { return currentTime; }
    
private:
    bool initializeGL();
    void processInput();
    void updateTime();
    void limitFrameRate();           // ֡������
    void updateFrameStats();         // ֡��ͳ��
    
    GLFWwindow* window;
    std::string title;
    int width, height;
    
    // �ص�����
    RenderCallback renderCallback;
    
    // ʱ����Ϣ
    double lastTime;
    double currentTime;
    double deltaTime;
    
    // ֡�ʿ���
    unsigned int targetFPS = 0;
    unsigned int currentFPS;
    unsigned int frameCount;
    double fpsUpdateTime;
    double frameStartTime;
    
    // ������Ϣ
    std::vector<bool> keys;
    std::vector<bool> mouseButtons;
    double mouseX; double mouseY;
};