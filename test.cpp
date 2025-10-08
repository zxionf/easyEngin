#include "engin/Application.hpp"

class Simple2DGame : public Application
{
public:
    Simple2DGame() : Application("null", 800, 640) {}
private:
    void render([[maybe_unused]] float deltaTime) override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 0, 1);
    }
};

int main()
{
    Simple2DGame game;
    game.run();
    return 0;
}