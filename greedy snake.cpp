#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>

using namespace std;

class SnakeGame {
private:
    enum Direction { STOP, UP, DOWN, LEFT, RIGHT }; // ����һ��ö����Direction�����ڱ�ʾ����
    
    static const int WIDTH = 40;  // ���ӿ��
    static const int HEIGHT = 25; // ���Ӹ߶�
    
    struct Point {
        int x;
        int y;
    };

    vector<Point> snake;  // ʹ��vector�洢��
    Point food;
    Direction dir;
    int sleep_time;
    int score;
    int Highscore;  //��ʷ��߷���
    bool gameBegin;  //�����ж���Ϸ��ʼ��������ְ����̳� ���治�����
    bool gameOver;
    char screen[HEIGHT][WIDTH]; // ˫������

    // �ƶ�����̨���λ��
    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    // ���������У�����һ����Ϸ�������ν���һ��ʱ��
    void clearline(int x) {
        for (int i = 0; i < x; ++i) {
            cout << "                                                                      " << endl; // ���70���ո�
        }
    }

    // ���ع��
    void hideCursor() {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(consoleHandle, &cursorInfo);
        cursorInfo.bVisible = false; // ���ù�겻�ɼ�
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }

    // ���ÿ���̨���ڴ�С
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
        //setConsoleSize(WIDTH * 8, HEIGHT * 16); // ���ÿ���̨���ڴ�С
        reset();
        sleep_time = 200;  //˯��ʱ���ʼ��
        Highscore = 0;  //HighscoreӦ����ʼ�������ǲ�����reset��ʼ������ΪHighscoreһֱ���浽�û��˳���Ϸ
    }

    void reset() {
        snake.clear();
        // ��ʼ����ͷλ��
        snake.push_back({WIDTH/2, HEIGHT/2});
        // ����ʳ��
        generateFood();
        // ��ʼ������
        dir = STOP;
        score = 0;
        gameBegin = true;
        gameOver = false;
    }

    void generateFood() {
        food.x = rand() % (WIDTH-2) + 1;
        food.y = rand() % (HEIGHT-2) + 1;
        // ȷ��ʳ�ﲻ��������
        for (const auto & seg : snake) {
            if (seg.x == food.x && seg.y == food.y) {
                generateFood();
                break;
            }
        }
    }

    void draw() { //output
        // �����Ļ������
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                screen[y][x] = ' ';
            }
        }

        // ���Ʊ߽�
        for (int i = 0; i < WIDTH; ++i) {
            screen[0][i] = '#';
            screen[HEIGHT-1][i] = '#';
        }
        for (int y = 1; y < HEIGHT-1; ++y) {
            screen[y][0] = '#';
            screen[y][WIDTH-1] = '#';
        }

        // ������ͷ
        screen[snake[0].y][snake[0].x] = 'O';

        // ����ʳ��
        screen[food.y][food.x] = '*';

        // ��������
        for (size_t i = 1; i < snake.size(); ++i) {
            screen[snake[i].y][snake[i].x] = 'o';
        }

        // ���������������������̨
        gotoxy(0, 0); // �ƶ���굽���Ͻ�
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                cout << screen[y][x];
            }
            cout << endl;
        }

        // ��ʾ����
        gotoxy(0, HEIGHT);
        clearline(1);
        gotoxy(0, HEIGHT);
        cout << "Score: " << score << '\t' << "History High Score: " << Highscore << endl;//���·��������У�д'\t'�Ʊ��������ԭ�����ݣ�ע��

        //��ȥ������ʾ
        if (!gameBegin) clearline(1);
    }

    void input() {
        if (_kbhit()) {
            gameBegin = false;//һ���û������˵�һ�ΰ�������Ϊ��Ϸ��ʼ��ȡ��������ʾ
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

        // �ƶ�����
        for (size_t i = snake.size()-1; i > 0; --i) {
            snake[i] = snake[i-1];
        }

        // �ƶ���ͷ
        switch (dir) {
            case UP:    --snake[0].y; break;
            case DOWN:  ++snake[0].y; break;
            case LEFT:  --snake[0].x; break;
            case RIGHT: ++snake[0].x; break;
        }

        // �����ײ
        // �߽���ײ
        if (snake[0].x <= 0 || snake[0].x >= WIDTH - 1 ||
            snake[0].y <= 0 || snake[0].y >= HEIGHT - 1) {
            gameOver = true;
            return;
        }

        // ������ײ
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                gameOver = true;
                return;
            }
        }

        // ��ʳ��
        if (snake[0].x == food.x && snake[0].y == food.y) {
            score += 10;
            snake.push_back(snake.back());
            generateFood();
        }
    }

    // ��Ϸ��ʼʱ�̣���ҽ���
    void gamestartInteract() {
        gotoxy(0, HEIGHT + 1);
        clearline(3);
        gotoxy(0, HEIGHT + 1);
        cout << "��ѡ��������Ϸ�Ѷȣ�1.�� 2.�е� 3.���ѣ����밢�������ֺ�س�" << endl;
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
        cout << "ѡ��ɹ�������W/A/S/D��ʼ��Ϸ";
    }

    // ���㻭��
    void stageClear() {
        gotoxy(0, HEIGHT);
        Highscore = max(Highscore, score);
        cout << "Score: " << score << '\t' << "History High Score: " << Highscore << endl;//���·���
        cout << "Game Over! Final Score: " << score;
        clearline(1);
        cout << "�Ƿ���Ҫ����һ�֣����ǣ�����1����������0��������»س�" << endl;
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
                update();//updateһ���ǽ�����input֮��draw������ǰ���ֹȱʧ��һ֡
                Sleep(sleep_time);
            }
            stageClear();
            cin >> continue_game;
        } while (continue_game);

        cout << "��Ϸ������������ʷ��ߵ÷�Ϊ " << Highscore << " �֣�" << endl;
        system("pause");
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}