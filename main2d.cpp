#include "engin/Application.hpp"
#include "engin/2d/Renderer2d.hpp"
#include "engin/2d/Sprite.hpp"
#include "engin/2d/Texture2d.hpp"
#include <memory>
#include <iostream>

class Simple2DGame : public Application {
public:
    Simple2DGame():Application(), renderer(*this) {
        if(!renderer.initialize())exit(1);
        setupGame();
        setRenderCallback([this](float dt) { this->render(dt);});
    }
    
private:
    void setupGame() {
        // ��������
        auto playerTexture = std::make_shared<Texture2D>();
        playerTexture->createSolidColor(32, 32, 255, 0, 0);
        
        auto enemyTexture = std::make_shared<Texture2D>();
        enemyTexture->createSolidColor(32, 32, 0, 0, 255);
        
        // ������Ҿ���
        player.setTexture(playerTexture);
        player.setPosition(Vector2<>(400, 300));
        player.setScale(Vector2<>(1, 1));
        player.setColor(1.0f, 1.0f, 1.0f);
        
        // �������˾���
        for (int i = 0; i < 5; ++i) {
            Sprite2D enemy;
            enemy.setTexture(enemyTexture);
            enemy.setPosition(Vector2<>(100 + i * 150, 200));
            enemy.setScale(Vector2<>(1, 1));
            enemy.setColor(1.0f, 1.0f, 1.0f);
            enemies.push_back(enemy);
        }
    }
    
    
    void render(float dt) {
// ��������
        double mouseX, mouseY;
        getMousePosition(mouseX, mouseY);
        
        // ������ƶ������λ��
        float playerX = static_cast<float>(mouseX);
        float playerY = static_cast<float>(mouseY);
        
        // ȷ����Ҳ����Ƴ���Ļ
        Vector2<> playerSize = player.getSize();
        playerX = std::max(playerSize.x * 0.5f, std::min(playerX, 800.0f - playerSize.x * 0.5f));
        playerY = std::max(playerSize.y * 0.5f, std::min(playerY, 600.0f - playerSize.y * 0.5f));
        
        player.setPosition(Vector2<>(playerX, playerY));
        
        // ��ת����
        static float totalTime = 0.0f;
        totalTime += dt;
        float rotation = totalTime * 90.0f; // ÿ��90��
        
        for (auto& enemy : enemies) {
            enemy.setRotation(rotation);
        }
        
        
        
        // ��ѡ����ʾ��ǰFPS��ÿ0.5�����һ�Σ�
        static float fpsTimer = 0.0f;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) {
            // std::cout << "FPS: " << getCurrentFPS() << std::endl;
            fpsTimer = 0.0f;
        }


        renderer.setClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.beginFrame();
        
        if(isKeyPressed(GLFW_KEY_D))std::cout << "sdsd" << std::endl;
        
        renderer.endFrame();

        player.rotate(this->getTime());
    }
    
private:
    Renderer2D renderer;
    Sprite2D player;
    std::vector<Sprite2D> enemies;
};

int main() {
    Simple2DGame game;
    game.run();
    return 0;
}