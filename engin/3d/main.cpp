#include "engin/Application.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameApp : public Application {
public:
    GameApp() : Application("Game Engine - Camera Demo", 800, 600), VAO(0), VBO(0) {}
    
protected:
    void initialize() override {
        std::cout << "Initializing game..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Mouse - Look around" << "  P - Toggle Perspective/Orthographic projection" << std::endl;
        std::cout << "  Mouse Wheel - Zoom" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        
        // ��ʼ����ɫ��
        shader.loadFromFile("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        
        // �������̸�����
        texture.createCheckerboard(256, 256, 32);
        
        // ��ʼ����� - ʹ��͸��ͶӰ
        camera.setPerspective(45.0f, static_cast<float>(getWidth()) / getHeight(), 0.1f, 100.0f);
        camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        
        // �������������
        setupScene();

        // ������̬����
    texture.createSolidColor(256, 256, glm::vec3(0.2f, 0.3f, 0.8f));
    
    // �������ϻ���ͼ��
    texture.drawRectangle(50, 50, 100, 100, glm::vec3(1.0f, 0.0f, 0.0f), true);
    texture.drawCircle(256, 256, 80, glm::vec3(0.0f, 1.0f, 0.0f), false);
    texture.drawLine(100, 400, 400, 100, glm::vec3(1.0f, 1.0f, 0.0f), 3);
    
    // Ӧ���˾�
    texture.applySepia();
    
    // ȷ�����µ�GPU
    texture.updateGPU();
        
        std::cout << "Game initialized successfully!" << std::endl;
    }
    
    void update(float deltaTime) override {
        // ��������ƶ�
        processCameraInput(deltaTime);
        
        // ������������ת
        static float rotationAngle = 0.0f;
        rotationAngle += 30.0f * deltaTime;
        
        for (auto& transform : cubeTransforms) {
            transform.setRotation(glm::vec3(rotationAngle, rotationAngle * 0.7f, 0.0f));
        }

        // ÿ�����޸�һ������
        static float timer = 0.0f;
    timer += deltaTime;
    {
        // ����޸�һ������
        int x = rand() % texture.getWidth();
        int y = rand() % texture.getHeight();
        glm::vec3 color(static_cast<float>(rand()) / RAND_MAX,
                       static_cast<float>(rand()) / RAND_MAX,
                       static_cast<float>(rand()) / RAND_MAX);
        
        texture.setPixel(x, y, color);
        texture.updateGPU();
    }
    }
    
    void render() override {
        // ���������
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // ʹ����ɫ��
        shader.use();
        
        // �����������
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);
        
        // ������
        texture.bind(0);
        shader.setUniform("texture1", 0);
        
        // ��Ⱦ����������
        glBindVertexArray(VAO);
        for (const auto& transform : cubeTransforms) {
            glm::mat4 model = transform.getModelMatrix();
            shader.setUniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        
        // ��ʾ��ǰͶӰģʽ
        renderDebugInfo();
    }
    
    void shutdown() override {
        std::cout << "Shutting down game..." << std::endl;
        
        // ����OpenGL��Դ
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        
        std::cout << "Game shutdown complete!" << std::endl;
    }
    
    void onKey(int key, int action) override {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(getWindow(), true);
                std::cout << "ESC pressed - Exiting game" << std::endl;
            }
            else if (key == GLFW_KEY_P) {
                camera.toggleProjectionMode();
            }
            else if (key == GLFW_KEY_F1) {
                std::cout << "Camera Position: (" 
                          << camera.getPosition().x << ", "
                          << camera.getPosition().y << ", "
                          << camera.getPosition().z << ")" << std::endl;
            }
        }
    }
    
    void onMouseMove(double x, double y) override {
        static bool firstMouse = true;
        static double lastX = 0.0, lastY = 0.0;
        
        if (firstMouse) {
            lastX = x;
            lastY = y;
            firstMouse = false;
            return;
        }
        
        double xoffset = x - lastX;
        double yoffset = lastY - y; // ��תY��
        
        lastX = x;
        lastY = y;
        
        camera.processMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }
    
    void onMouseButton(int button, int action) override {
        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                std::cout << "Left mouse button pressed" << std::endl;
            }
        }
    }
    
private:
    void setupScene() {
        setupVertexData();
        
        // ������������壬�����ڳ�����
        cubeTransforms.clear();
        
        // ����������
        Transform centerCube;
        centerCube.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        centerCube.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
        cubeTransforms.push_back(centerCube);
        
        // ��Χ��������
        float radius = 3.0f;
        for (int i = 0; i < 8; ++i) {
            float angle = glm::radians(45.0f * i);
            Transform cube;
            cube.setPosition(glm::vec3(
                radius * cos(angle),
                0.0f,
                radius * sin(angle)
            ));
            cube.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
            cubeTransforms.push_back(cube);
        }
        
        // ���µ�������
        Transform topCube;
        topCube.setPosition(glm::vec3(0.0f, radius, 0.0f));
        topCube.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        cubeTransforms.push_back(topCube);
        
        Transform bottomCube;
        bottomCube.setPosition(glm::vec3(0.0f, -radius, 0.0f));
        bottomCube.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        cubeTransforms.push_back(bottomCube);
    }
    
    void setupVertexData() {
        // �����嶥�����ݣ�λ�� + �������꣩
        float vertices[] = {
            // ����
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            
            // ǰ��
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            
            // ����
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            
            // ����
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            
            // ����
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            
            // ����
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        
        // ���ɶ����������Ͷ��㻺�����
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        // ��VAO
        glBindVertexArray(VAO);
        
        // ��VBO���ϴ�����
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // λ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // ������������
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // ���
        glBindVertexArray(0);
    }
    
    void processCameraInput(float deltaTime) {
        // WASD ��������ƶ�
        if (glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(Camera::FORWARD, deltaTime);
        if (glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(Camera::BACKWARD, deltaTime);
        if (glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(Camera::LEFT, deltaTime);
        if (glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard(Camera::RIGHT, deltaTime);
    }
    
    void renderDebugInfo() {
        // �ڿ���̨��ʾ�����Ϣ����ʵ����Ŀ�п���ʹ��ImGui��GUI�⣩
        static int frameCount = 0;
        frameCount++;
        
        if (frameCount % 60 == 0) { // ÿ�����һ��
            std::cout << "\rProjection: " 
                      << (camera.getProjectionMode() == Camera::PERSPECTIVE ? "Perspective" : "Orthographic")
                      << " | Position: (" 
                      << camera.getPosition().x << ", "
                      << camera.getPosition().y << ", "
                      << camera.getPosition().z << ")"
                      << std::flush;
        }
    }
    
private:
    Shader shader;
    Texture texture;
    Camera camera;
    std::vector<Transform> cubeTransforms;
    
    GLuint VAO, VBO;
};

int main() {
    GameApp app;
    app.run();
    return 0;
}