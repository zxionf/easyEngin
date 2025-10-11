# easyEngine

һ������ OpenGL �� C++17 �������� 2D ��Ϸ���棬����̰������Ϸʾ����

## ��Ŀ����

����һ��ģ�黯�� 2D ��Ϸ���棬�ṩ����Ⱦ�����봦����������������Ⱥ��Ĺ��ܡ�����ʹ���ִ� C++ ��д���������������ƣ�������չ��ʹ�á�

Ŀǰ�ǿ����׶Σ���๦�ܲ�����Ҳ�����ơ�

## ��������

### ����
- **��ƽ̨֧��**������ GLFW �� OpenGL��֧�� Windows��Linux��macOS
- **�ִ� C++**��ʹ�� C++17 ��׼����������ָ�롢lambda ���ʽ���ִ�����
- **֡�ʿ���**�������õ�֡�����ƣ�ȷ����Ϸ����һ���ԣ�ʵ���ԣ�
- **2D ������Ⱦ**��֧��λ�á����š���ת����ɫ����
- **�������**��֧�ִ��ļ���������������������
- **����ͼ�λ���**�����Ρ�������Բ�εȻ���ͼ�λ���
- **��������Ⱦ**���Ż�����Ⱦ������ϵͳ
- **���ϵͳ**��2D ���֧��ƽ�ơ����ź���ת
- **��������**��ʵʱ����״̬���
- **�������**�����λ�úͰ�ť״̬��ȡ
- **�ص�ϵͳ**�������¼��ص�����

### ʾ��
����չʾ�������湦��  

| | |
|--|--|
|̰����<br>![doc/demo_snake.png](./doc/demo_snake.png)|ɨ��<br>![doc/demo_minesweeper.png](./doc/demo_minesweeper.png)|


## ��Ŀ�ṹ
```
game/
������ libs/                           # ����̬��
������ deps/                           # ����
������ engin/                          # ������Ĵ���
��   ������ Application.hpp/cpp         # Ӧ�ù������Ϸѭ��
��   ������ 2d/                         # 2D ��Ⱦģ��
��   ��   ������ Renderer2d.hpp/cpp      # 2D ��Ⱦ��
��   ��   ������ Sprite.hpp/cpp          # ������
��   ��   ������ Texture2d.hpp/cpp       # �������
��   ��   ������ Camera2d.hpp/cpp        # 2D ���
��   ��   ������ Vector2.hpp             # 2D ������ѧ
��   ��   ������ Rect.hpp                # ���κ���ײ���
������ examples/                       # ʾ������
��   ������ snake.cpp                   # ̰����
��   ������ minesweeper.cpp             # ɨ��
������ doc/                            # ��ͼ���ĵ�
������ CMakeLists.txt                  # ��������
������ assets/                         # ��Դ�ļ�
    ������ textures/                   # ����
    ������ shaders/                    # GLSL ��ɫ��
```

## ���ٿ�ʼ
```cpp
#include "engin/Application.hpp"

class Simple2DGame : public Application
{
public:
    Simple2DGame() : Application("null", 800, 640) {}
private:
    void render([[maybe_unused]] float dt) override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 0.0, 1.0);
    }
};

int main()
{
    Simple2DGame game;
    game.run();
    return 0;
}
```
what we get?
![doc/demo_quickstart.png](./doc/demo_quickstart.png)

## Ŀǰ����
- ���ص�ͼƬ������������
- ֡�ʿ��ƴ����������ȱ��
- �޷���������
- ...