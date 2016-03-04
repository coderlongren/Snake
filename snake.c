#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define WIDTH 80
#define HEIGHT 40

int finish = 0;
// 表示世界
int world[ HEIGHT ][ WIDTH ];  // 贪吃蛇所在的世界舞台

// 表示贪吃蛇的身体
int L, D;  // 贪吃蛇长度，方向
int X[ WIDTH * HEIGHT ]; // 贪吃蛇每节身体的横坐标
int Y[ WIDTH * HEIGHT ]; // 贪吃蛇每节身体的纵坐标

// 表示贪吃蛇的方向, 上下左右
enum direction {
    UP, RIGHT, DOWN, LEFT
};
char *tips[4] = {
    "up", "right", "down", "left"
};
int DX[4] = {0, 1, 0, -1};
int DY[4] = {-1, 0, 1, 0};

struct Food_ {
    int x, y;
} food;   // 代表食物所在坐标位置

void clear();  // 清空屏幕

void init();   // 初始化
void draw();   // 将游戏场景画到cli

void random_food();  // 当食物倍吃时，随机出一块新的食物
void adjust();  // 模拟贪吃蛇的下一步

int main() {
    char ch;

    // 初始化世界
    init();
    while (!finish) {
        // 处理事件
        if ((ch = getchar()) > 0) {
            printf("get char %d\n", ch);
            switch (ch) {
                case 'q':
                    finish = 1;
                    break;
                case 'w':
                    D = UP;
                    break;
                case 's':
                    D = DOWN;
                    break;
                case 'a':
                    D = LEFT;
                    break;
                case 'd':
                    D = RIGHT;
                    break;
            }
        }
        while(getchar() > 0);
    
        // 模拟下一步要发生的事情
        adjust();

        // 绘制世界
        draw();

        printf("当前方向: %s. 提示：w+enter: up, s+enter: down, "
                "a+enter: left, d+enter: right, q+enter: exit\n", tips[D]);
        usleep(600000);
    }

    printf("Game Over!\n");
    return EXIT_SUCCESS;
}

void init() {
    int i, flags, d;

    // 设置stdin阻塞IO
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL,flags);

    // 初始化贪吃蛇的头在世界中心
    memset(X, 0, sizeof(X));
    memset(Y, 0, sizeof(Y));
    X[0] = WIDTH / 2 - 1;
    Y[0] = HEIGHT / 2 - 1;
    srandom(time(NULL));
    D = random() % 4;
    d = (D + 2) % 4; // D的反方向，初始化贪吃蛇有3节身体
    for (i = 1; i <= 3; ++i) {
        X[i] = X[i - 1] + DX[d];
        Y[i] = Y[i - 1] + DY[d];
    }
    L = 4;  // 算上头部，贪吃蛇一共长4

    // 初始化第一个食物位置
    random_food();
}

void random_food() {
    int i;
    int ok = 0;
    do {
        food.x = (random() % (WIDTH - 2)) + 1;
        food.y = (random() % (HEIGHT - 2)) + 1;

        ok = 1;
        for (i = 0; i < L; i++) {
            if (X[i] == food.x && Y[i] == food.y) {
                ok = 0;
                break;
            }
        }
    } while (!ok);
}

void adjust() {
    int x, y, i;
    int did_knock_wall, did_knock_self;
    x = X[0] + DX[ D ];
    y = Y[0] + DY[ D ];

    for (i = L; i > 0; --i) {
        X[i] = X[i - 1];
        Y[i] = Y[i - 1];
    }
    X[0] = x;
    Y[0] = y;
    // 判断是否吃到了食物
    if (x == food.x && y == food.y) {
        L ++;
        random_food();
    } else {
        // 检测碰撞
        did_knock_wall = (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1);
        did_knock_self = 0;
        for (i = 1; i < L; ++i) {
            if (x == X[i] && y == Y[i]) {
                did_knock_self = 1;
                break;
            }
        }

        if (did_knock_self || did_knock_wall) {
            finish = 1;
        }
    }
}

void draw() {
    int i, j;

    // 初始化世界
    memset(world, 0, sizeof(world));
    for (i = 0; i < WIDTH; i++) {
        world[0][i] = '#';
        world[ HEIGHT - 1 ][i] = '#';
    }
    for (i = 1; i < HEIGHT - 1; i++) {
        world[i][0] = '#';
        world[i][ WIDTH - 1 ] = '#';
    }

    // 填充世界
    world[ food.y ][ food.x ] = 'B';
    world[ Y[0] ][ X[0] ] = '@';
    for (i = 1; i < L; ++i) {
        world[ Y[i] ][ X[i] ] = 'H';
    }

    clear();
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (world[i][j] > 0) 
                putchar(world[i][j]);
            else
                putchar(' ');
        }
        printf("\n");
    }
}

void clear() {
    printf("\033[H\033[J");
}
