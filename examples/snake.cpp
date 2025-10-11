#include "engin/Application.hpp"
#include "engin/2d/Renderer2d.hpp"
#include "engin/2d/Sprite.hpp"
#include "engin/2d/Texture2d.hpp"
#include <memory>
#include <iostream>

class Simple2DGame : public Application
{
private:
    Renderer2D renderer;
    Vector2<int> apple;
    Vector2<int> direct;
    struct Snake
    {
        Vector2<int> dot;
        Vector2<int> dotdirect;
    } head{{10,10},{0,0}};

    std::vector<Snake> snake;
    bool running = true;

public:
    Simple2DGame() : Application("null", 800, 640), renderer(*this)
    {
        snake.push_back(head);
        srand(static_cast<unsigned int>(getTime()));
        refreshApplePos();
    }

private:
    void render([[maybe_unused]] float dt) override
    {
        renderer.setClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (snake.at(0).dot.x == apple.x && snake.at(0).dot.y == apple.y)
        {
            refreshApplePos();
            Snake newdot;
            newdot.dot = snake.at(snake.size() - 1).dot;
            newdot.dotdirect = snake.at(snake.size() - 1).dotdirect;
            snake.push_back(newdot);
        }

        static float fpsTimer = 0.0f;
        fpsTimer += dt;
        if (running && fpsTimer >= 0.09f)
        {

            // std::cout << snake.at(0).dot.x << "\\" << head.dot.y << std::endl;
            for (int i = snake.size() - 1; i > 0; i--)
            {
                snake.at(i).dotdirect = snake.at(i - 1).dotdirect;
                snake.at(i).dot = snake.at(i - 1).dot;
            }
            if(!((snake.at(0).dotdirect+direct)==Vector2<int>::zero()))snake.at(0).dotdirect = direct;
            snake.at(0).dot += snake.at(0).dotdirect;
            


            //±ß½ç´¦Àí
            if(snake.at(0).dot.x>25) snake.at(0).dot.x = 0;
            if(snake.at(0).dot.x<0) snake.at(0).dot.x = 25;

            if(snake.at(0).dot.y>19) snake.at(0).dot.y = 0;
            if(snake.at(0).dot.y<0) snake.at(0).dot.y = 19;

            for(int i = snake.size() - 1; i > 0; i--){
                if(snake.at(i).dot == snake.at(0).dot){
                    running = false;
                    std::cout << "\n\n\n\n \033[0m\033[1;31m you failed \033[0m \n "
                    << " score:"
                    << snake.size()
                    << "\n\n\n" << std::endl;
                }
            }

            fpsTimer = 0.0f;
        }

        renderer.beginFrame();

        // if(isKeyPressed(GLFW_KEY_D))apple.x+=100.0 * dt;
        if (isKeyPressed(GLFW_KEY_W))
            direct = Vector2<int>::down();
        if (isKeyPressed(GLFW_KEY_A))
            direct = Vector2<int>::left();
        if (isKeyPressed(GLFW_KEY_D))
            direct = Vector2<int>::right();
        if (isKeyPressed(GLFW_KEY_S))
            direct = Vector2<int>::up();

        renderer.drawRectangle(apple.x * 32, apple.y * 32, 32, 32, 0.6, 0.0, 0.0);
        for (Snake s : snake)
            renderer.drawRectangle(s.dot.x * 32, s.dot.y * 32, 32, 32, 0.8, 0.8, 0.0);

        renderer.endFrame();
    }

    void refreshApplePos()
    {
        do{
            
            apple.x = rand() % 25;
            apple.y = rand() % 18;
        } while ([this]() -> bool
                 {
                     for (const Snake &v : snake)
                     {
                         if (v.dot.x == apple.x && v.dot.y == apple.y)
                             return true;
                     }
                     return false;
                 }());
    }
};

int main()
{
    Simple2DGame game;
    game.run();
    return 0;
}