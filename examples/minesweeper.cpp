#include <engin/Application.hpp>
#include <engin/2d/Renderer2d.hpp>
#include <engin/2d/Vector2.hpp>
#include <iostream>
#include <vector>

class Simple2DGame : public Application
{
private:
    Renderer2D renderer;
    std::vector<Sprite2D> nums;
    Sprite2D bom, flag;
    bool first = true;
    bool running = true;
    float timeover = 0;
    int w = 0;

    int map[24][18][3] = {0};
    // 顶层 ren
    // 1层 flag
    // 0层 炸弹,数字
    //flag  -3
    // bom  -2
    // w    -1
    // null  0
    // 1     1
public:
    Simple2DGame() : Application("minesweeper", 768, 576), renderer(*this)
    {
        setupGame();
        srand(time(NULL));
        putBoms(10);
    }

private:
    void render([[maybe_unused]] float deltaTime) override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0, 1);

        double mouseX, mouseY;
        getMousePosition(mouseX, mouseY);
        timeover += deltaTime;
        if (running)
            updateTopLayer(mouseX, mouseY);

        renderer.setClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        renderer.beginFrame();
        // renderer.drawSprite(background);
        for (int i = 0; i < 24; i++)
            for (int j = 0; j < 18; j++)
            {
                if (first){
                    map[i][j][1] = -1;
                    map[i][j][2] = -1;
                }
                if (map[i][j][2] > 0 && map[i][j][2] < 9)rend(nums.at(map[i][j][2]), i, j);
                if (map[i][j][2] == -1)rend(nums.at(0), i, j);
                if (map[i][j][2] == -2)rend(bom, i, j);
                if (map[i][j][2] == -3)rend(flag, i, j);
            }
        // renderer.drawSprite(bom);
        renderer.endFrame();

        first = false;
    }

    void updateTopLayer(double &mouseX, double &mouseY)
    {
        if(timeover<0.1)return;
        else timeover=0;
        int x = (int)mouseX / 32;
        int y = (int)mouseY / 32;
        if (isMouseButtonPressed(0))
        { // 按下左键
            if (map[x][y][0] == -2)
            {
                running = false;
                map[x][y][2] = -2; // 显示炸弹（failed）
                std::cout << "\n\n\n\n \033[0m\033[1;31m you failed \033[0m \n "
                    << "\n\n\n" << std::endl;
                return;
            }
            else
            {
                check(x, y);// 会更新顶层
            }
        }
        else if (isMouseButtonPressed(1))
        {
            // 变换旗子
            if(map[x][y][2] == -1) {
                map[x][y][2] = -3;// 更新顶层旗子
            }else if(map[x][y][2] == -3){
                map[x][y][2] = -1;// 更新顶层旗子
            }
        }
        // 检测通关条件
            w=0;
            for (int i = 0; i < 24; i++)
                for (int j = 0; j < 18; j++){
                    if(map[i][j][2] == -3&& map[i][j][0] == -2) w++;
                    if(map[i][j][2] == -1) w++;
                }
            
            if(w==10){
                running = false;
                std::cout << "\n\n\n\n win \n\n\n\n" << std::endl;
            }
    }

    void check(int x, int y)
    {
        if (x < 0 || x > 23 || y < 0 || y > 17)return;
        if (map[x][y][2] == 0)return;
        if (map[x][y][0] == -2)return;
        map[x][y][2] = map[x][y][0];
        if (map[x][y][0] != 0)return;

        else
        {
            check(x + 1, y);
            check(x, y + 1);
            check(x - 1, y);
            check(x, y - 1);
        }
    }

    void putBoms(int num)
    {
        int x, y;
        for (int i = 0; i < num; i++)
            do
            {
                x = rand() % 24;
                y = rand() % 18;
            } while ([this, &x, &y]() -> bool
                     {
                if(map[x+1][y+1][0] == -2) return true;
                else {
                    map[x+1][y+1][0] = -2;
                    return false;
                } }());

        for (int i = 0; i < 24; i++)
        {
            for (int j = 0; j < 18; j++)
            {
                // std::cout << map[i][j][2] << " ";
                if (map[i][j][0] != -2)
                {
                    if(i-1>=0)map[i][j][0] += map[i - 1][j][0] == -2;
                    if(j-1>=0)map[i][j][0] += map[i][j - 1][0] == -2;
                    if(i+1<=23)map[i][j][0] += map[i + 1][j][0] == -2;
                    if(j+1<=17)map[i][j][0] += map[i][j + 1][0] == -2;
                    if(i-1>=0&&j-1>=0)map[i][j][0] += map[i - 1][j - 1][0] == -2;
                    if(i+1<=23&&j+1<=17)map[i][j][0] += map[i + 1][j + 1][0] == -2;
                    if(i-1>=0&&j+1<=17)map[i][j][0] += map[i - 1][j + 1][0] == -2;
                    if(i+1<=230&&j-1>=0)map[i][j][0] += map[i + 1][j - 1][0] == -2;
                }
            }
            // std::cout << std::endl;
        }
    }

    void rend(Sprite2D &sprite, int x, int y)
    {
        sprite.setPosition(Vector2<>(x * 32, y * 32 + 32));
        sprite.setScale(Vector2<>(1, -1));
        renderer.drawSprite(sprite);
    }

    void setupGame()
    {
        // 尝试从文件加载纹理
        auto texture_w = std::make_shared<Texture2D>();
        texture_w->createFromFile("assets/textures/w.png");
        nums.push_back(*(new Sprite2D(texture_w)));
        for (int i = 1; i < 9; i++)
            nums.push_back(*(new Sprite2D(std::make_shared<Texture2D>("assets/textures/" + std::to_string(i) + ".png"))));

        bom.setTexture(std::make_shared<Texture2D>("assets/textures/bom.png"));
        flag.setTexture(std::make_shared<Texture2D>("assets/textures/flag.png"));
    }
};

int main()
{
    Simple2DGame game;
    game.run();
    return 0;
}