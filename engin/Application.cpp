#include "Application.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>  // 用于Sleep函数
#else
#include <unistd.h>   // 用于usleep函数
#endif

Application::Application(const std::string& title, int width, int height)
    : window(nullptr),title(title), width(width), height(height),
      lastTime(0), currentTime(0), deltaTime(0),
      targetFPS(0), currentFPS(0), frameCount(0), fpsUpdateTime(0),
      mouseX(0), mouseY(0) {
    
    keys.resize(GLFW_KEY_LAST + 1, false);
    mouseButtons.resize(GLFW_MOUSE_BUTTON_LAST + 1, false);

    this->width = width;
    this->height = height;
    
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }
    
    // 创建窗口
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    
    glfwMakeContextCurrent(window);
    
    // 禁用垂直同步，使用我们自己的帧率控制
    glfwSwapInterval(0);
    
    // 初始化GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
    // 设置OpenGL状态
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // 2D不需要深度测试
    
    std::cout << "2D Engine initialized successfully!" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Target FPS: " << targetFPS << std::endl;
}

Application::~Application() {
    shutdown();
}

void Application::run() {
    lastTime = glfwGetTime();
    fpsUpdateTime = lastTime;
    
    while (!shouldClose()) {
        // 记录帧开始时间
        frameStartTime = glfwGetTime();
        
        updateTime();
        processInput();
        
        // 调用渲染回调
        if (renderCallback) {
            renderCallback(static_cast<float>(deltaTime));
        }else{
            render(deltaTime);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // 帧率控制和统计
        limitFrameRate();
        updateFrameStats();
    }
}

void Application::render([[maybe_unused]] float deltaTime){

}

void Application::setTargetFPS(unsigned int fps) {
    if (fps > 0) {
        targetFPS = fps;
        std::cout << "Target FPS set to: " << targetFPS << std::endl;
    }
}

void Application::limitFrameRate() {
    if (targetFPS == 0) return; // 0表示无限制
    
    // 计算目标帧时间（秒）
    double targetFrameTime = 1.0 / targetFPS;
    
    // 计算当前帧实际用时
    double frameEndTime = glfwGetTime();
    double frameDuration = frameEndTime - frameStartTime;
    
    // 如果帧用时小于目标时间，则等待剩余时间
    if (frameDuration < targetFrameTime) {
        double sleepTime = targetFrameTime - frameDuration;
        
        // 使用简单的忙等待循环，比sleep更精确
        double waitUntil = frameEndTime + sleepTime;
        while (glfwGetTime() < waitUntil) {
            // 空循环等待 - 这比sleep更精确
            // 如果需要减少CPU使用率，可以添加一个小的sleep
            if (sleepTime > 0.001) { // 如果剩余时间大于1ms
                // 在Windows上使用Sleep，其他平台使用usleep
                #ifdef _WIN32
                    Sleep(1); // 1毫秒
                #else
                    usleep(1000); // 1000微秒 = 1毫秒
                #endif
            }
        }
    }
}

void Application::updateFrameStats() {
    frameCount++;
    
    // 每秒更新一次FPS计数
    double currentTime = glfwGetTime();
    if (currentTime - fpsUpdateTime >= 1.0) {
        currentFPS = frameCount;
        frameCount = 0;
        fpsUpdateTime = currentTime;
        
        // 在控制台输出FPS（调试用）
        std::cout << "\rFPS: " << currentFPS << std::flush;
    }
}

void Application::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool Application::shouldClose() const {
    return window ? glfwWindowShouldClose(window) : true;
}

bool Application::isKeyPressed(int key) const {
    return key >= 0 && static_cast<size_t>(key) < keys.size() ? keys[key] : false;
}

bool Application::isMouseButtonPressed(int button) const {
    return button >= 0 && static_cast<size_t>(button) < mouseButtons.size() ? mouseButtons[button] : false;
}

void Application::getMousePosition(double& x, double& y) const {
    x = mouseX;
    y = mouseY;
}

void Application::processInput() {
    // 更新按键状态
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        keys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
    
    // 更新鼠标状态
    for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
        mouseButtons[button] = glfwGetMouseButton(window, button) == GLFW_PRESS;
    }
    
    // 更新鼠标位置
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // ESC键退出
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::updateTime() {
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}