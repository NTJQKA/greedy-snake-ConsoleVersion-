#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;

class SnakeGame {
private:
    enum Direction { STOP, UP, DOWN, LEFT, RIGHT }; // 定义一个枚举类Direction，用于表示方向
    
    static const int WIDTH = 40;  // 增加宽度
    static const int HEIGHT = 25; // 增加高度
    
    struct Point {
        int x;
        int y;
    };

    vector<Point> snake;  // 使用vector存储蛇
    Point food;
    Direction dir;
    int sleep_time;
    int score;
    int Highscore;  //历史最高分数
    bool gameBegin;  //用于判断游戏开始并输出新手按键教程 后面不再输出
    bool gameOver;
    char screen[HEIGHT][WIDTH]; // 双缓冲区

    // 移动控制台光标位置
    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    // 清理若干行（用于一句游戏结束，衔接下一场时）
    void clearline(int x) {
        for (int i = 0; i < x; ++i) {
            cout << "                                                                      " << endl; // 输出70个空格
        }
    }

    // 隐藏光标
    void hideCursor() {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(consoleHandle, &cursorInfo);
        cursorInfo.bVisible = false; // 设置光标不可见
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }

    // 设置控制台窗口大小
    void setConsoleSize(int width, int height) {
        HWND console = GetConsoleWindow();
        RECT r;
        GetWindowRect(console, &r);
        MoveWindow(console, r.left, r.top, width, height, TRUE);
    }

public:
    SnakeGame() : score(0), gameOver(false) {
        snake.reserve(100);
        hideCursor();
        //setConsoleSize(WIDTH * 8, HEIGHT * 16); // 设置控制台窗口大小
        reset();
        sleep_time = 200;  //睡眠时间初始化
        Highscore = 0;  //Highscore应当初始化，但是不跟随reset初始化，因为Highscore一直保存到用户退出游戏
    }

    void reset() {
        snake.clear();
        // 初始化蛇头位置
        snake.push_back({WIDTH/2, HEIGHT/2});
        // 生成食物
        generateFood();
        // 初始化方向
        dir = STOP;
        score = 0;
        gameBegin = true;
        gameOver = false;
    }

    void generateFood() {
        food.x = rand() % (WIDTH-2) + 1;
        food.y = rand() % (HEIGHT-2) + 1;
        // 确保食物不在蛇身上
        for (const auto & seg : snake) {
            if (seg.x == food.x && seg.y == food.y) {
                generateFood();
                break;
            }
        }
    }

    void draw() { //output
        // 清空屏幕缓冲区
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                screen[y][x] = ' ';
            }
        }

        // 绘制边界
        for (int i = 0; i < WIDTH; ++i) {
            screen[0][i] = '#';
            screen[HEIGHT-1][i] = '#';
        }
        for (int y = 1; y < HEIGHT-1; ++y) {
            screen[y][0] = '#';
            screen[y][WIDTH-1] = '#';
        }

        // 绘制蛇头
        screen[snake[0].y][snake[0].x] = 'O';

        // 绘制食物
        screen[food.y][food.x] = '*';

        // 绘制蛇身
        for (size_t i = 1; i < snake.size(); ++i) {
            screen[snake[i].y][snake[i].x] = 'o';
        }

        // 将缓冲区内容输出到控制台
        gotoxy(0, 0); // 移动光标到左上角
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                cout << screen[y][x];
            }
            cout << endl;
        }

        // 显示分数
        gotoxy(0, HEIGHT);
        clearline(1);
        gotoxy(0, HEIGHT);
        cout << "Score: " << score << '\t' << "History High Score: " << Highscore << endl;//更新分数，其中，写'\t'制表符不覆盖原有内容，注意

        //撤去按键提示
        if (!gameBegin) clearline(1);
    }

    void input() {
        if (_kbhit()) {
            gameBegin = false;//一旦用户进行了第一次按键，视为游戏开始并取消按键提示
            switch (_getch()) {
                case 'w': if (dir != DOWN) dir = UP; break;
                case 's': if (dir != UP) dir = DOWN; break;
                case 'a': if (dir != RIGHT) dir = LEFT; break;
                case 'd': if (dir != LEFT) dir = RIGHT; break;
                case 'x': gameOver = true; break;
            }
        }
    }

    void update() {
        if (dir == STOP) return;

        // 移动蛇身
        for (size_t i = snake.size()-1; i > 0; --i) {
            snake[i] = snake[i-1];
        }

        // 移动蛇头
        switch (dir) {
            case UP:    --snake[0].y; break;
            case DOWN:  ++snake[0].y; break;
            case LEFT:  --snake[0].x; break;
            case RIGHT: ++snake[0].x; break;
        }

        // 检测碰撞
        // 边界碰撞
        if (snake[0].x <= 0 || snake[0].x >= WIDTH - 1 ||
            snake[0].y <= 0 || snake[0].y >= HEIGHT - 1) {
            gameOver = true;
            return;
        }

        // 自身碰撞
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                gameOver = true;
                return;
            }
        }

        // 吃食物
        if (snake[0].x == food.x && snake[0].y == food.y) {
            score += 10;
            snake.push_back(snake.back());
            generateFood();
        }
    }

    // 游戏开始时刻，玩家交互
    void gamestartInteract() {
        gotoxy(0, HEIGHT + 1);
        clearline(3);
        gotoxy(0, HEIGHT + 1);
        cout << "请选择您的游戏难度：1.简单 2.中等 3.困难，输入阿拉伯数字后回车" << endl;
        int opt = 0;
        cin >> opt;
        switch (opt) {
            case 1 : sleep_time = 300; break;
            case 2 : sleep_time = 200; break;
            case 3 : sleep_time = 100; break;
        }
        gotoxy(0, HEIGHT + 1);
        clearline(2);
        gotoxy(0, HEIGHT + 1);
        cout << "选择成功，按下W/A/S/D开始游戏";
    }

    // 结算画面
    void stageClear() {
        gotoxy(0, HEIGHT);
        Highscore = max(Highscore, score);
        cout << "Score: " << score << '\t' << "History High Score: " << Highscore << endl;//更新分数
        cout << "Game Over! Final Score: " << score;
        clearline(1);
        cout << "是否想要再来一局？若是，输入1，否则，输入0，输入后按下回车" << endl;
    }

    void run() {
        srand(static_cast<unsigned>(time(nullptr)));
        int continue_game = 0;
        do {
            reset();
            draw();
            gamestartInteract();
            while (!gameOver) {
                draw();//output to screen
                input();
                update();//update一定是紧随在input之后，draw放在最前面防止缺失第一帧
                Sleep(sleep_time);
            }
            stageClear();
            cin >> continue_game;
        } while (continue_game);

        cout << "游戏结束，您的历史最高得分为 " << Highscore << " 分！" << endl;
        system("pause");
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}