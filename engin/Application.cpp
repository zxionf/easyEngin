#include "Application.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>  // ����Sleep����
#else
#include <unistd.h>   // ����usleep����
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
    
    // ��ʼ��GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }
    
    // ��������
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    
    glfwMakeContextCurrent(window);
    
    // ���ô�ֱͬ����ʹ�������Լ���֡�ʿ���
    glfwSwapInterval(0);
    
    // ��ʼ��GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
    // ����OpenGL״̬
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // 2D����Ҫ��Ȳ���
    
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
        // ��¼֡��ʼʱ��
        frameStartTime = glfwGetTime();
        
        updateTime();
        processInput();
        
        // ������Ⱦ�ص�
        if (renderCallback) {
            renderCallback(static_cast<float>(deltaTime));
        }else{
            render(deltaTime);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // ֡�ʿ��ƺ�ͳ��
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
    if (targetFPS == 0) return; // 0��ʾ������
    
    // ����Ŀ��֡ʱ�䣨�룩
    double targetFrameTime = 1.0 / targetFPS;
    
    // ���㵱ǰ֡ʵ����ʱ
    double frameEndTime = glfwGetTime();
    double frameDuration = frameEndTime - frameStartTime;
    
    // ���֡��ʱС��Ŀ��ʱ�䣬��ȴ�ʣ��ʱ��
    if (frameDuration < targetFrameTime) {
        double sleepTime = targetFrameTime - frameDuration;
        
        // ʹ�ü򵥵�æ�ȴ�ѭ������sleep����ȷ
        double waitUntil = frameEndTime + sleepTime;
        while (glfwGetTime() < waitUntil) {
            // ��ѭ���ȴ� - ���sleep����ȷ
            // �����Ҫ����CPUʹ���ʣ��������һ��С��sleep
            if (sleepTime > 0.001) { // ���ʣ��ʱ�����1ms
                // ��Windows��ʹ��Sleep������ƽ̨ʹ��usleep
                #ifdef _WIN32
                    Sleep(1); // 1����
                #else
                    usleep(1000); // 1000΢�� = 1����
                #endif
            }
        }
    }
}

void Application::updateFrameStats() {
    frameCount++;
    
    // ÿ�����һ��FPS����
    double currentTime = glfwGetTime();
    if (currentTime - fpsUpdateTime >= 1.0) {
        currentFPS = frameCount;
        frameCount = 0;
        fpsUpdateTime = currentTime;
        
        // �ڿ���̨���FPS�������ã�
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
    // ���°���״̬
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        keys[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
    
    // �������״̬
    for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
        mouseButtons[button] = glfwGetMouseButton(window, button) == GLFW_PRESS;
    }
    
    // �������λ��
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // ESC���˳�
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::updateTime() {
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}