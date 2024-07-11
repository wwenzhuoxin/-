#include "global.h"
#include "chess_io.h"
#include "tools.h"

char *PlayerInput() {
    char *input = (char *)malloc(sizeof(char) * 10);
    scanf("%s", input);
    return input;
}

Cmd init_player_command(int command_type, ...) {
    va_list args;

    Cmd cmd = (Cmd)malloc(sizeof(struct PlayerCommand));
    cmd->command_type = command_type;
    return cmd;
}

void clearScreen(){    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };    // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.

    // Put the cursor at its home coordinates.
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void fillScreenWithBlank() {
    for (int i = 0; i < 50; i++)
    {
        putchar(' ');
    }
    putchar('\n');
}

/**
 * @brief 设置文字颜色
 * 
 * @param x 颜色代码
 */
void color(int x) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
}

int color_printf(int text_color, const char *format, ...) {
    color(text_color);
    va_list args;           // 定义一个va_list类型的变量，用来储存单个参数
    va_start(args, format); // 使args指向可变参数的第一个参数
    int len = vprintf(format, args);  // 必须用带v的
    va_end(args);           // 结束可变参数的获取
    color(WHITE_TEXT);
    return len;
}

int autoColor_printf(int user, const char *format, ...) {
    int text_color = WHITE_TEXT;
    if (user == PLAYER_1) text_color = RED_TEXT;
    if (user == PLAYER_2) text_color = GREEN_TEXT;
    color(text_color);
    va_list args;           // 定义一个va_list类型的变量，用来储存单个参数
    va_start(args, format); // 使args指向可变参数的第一个参数
    int len = vprintf(format, args);  // 必须用带v的
    va_end(args);           // 结束可变参数的获取
    color(WHITE_TEXT);
    return len;
}

void printChessBoard(struct ChessBoard* board) {
    // cls;
    clearScreen(); // 更新界面，windows为"cls"，linux为""

    printf("\n=================================================================\n");
    printf("当前棋盘上的棋子：\n");
    for (int i = 0, len = 0; i <= board->objects->top; i++, len++)
    {
        if (len >= 8) {
            len = 0;
            putchar('\n');
        }
        autoColor_printf(board->objects->stack[i]->owner, "%s(%d)\t", chessName(board->objects->stack[i]), i);
    }
    printf("\n=================================================================\n");

    printf("  ");
    for (int i = 0; i < 9; i++)
    {
        printf("%d  ", i);
    }
    putchar('\n');  
    
    for (int i = 0; i < BOARD_ROW; i++)
    {
        printf("%d ", i);
        for (int j = 0; j < BOARD_COL; j++)
        {
            if(board->block[i][j] == NULL) {
                if (board->moveablePos != NULL && board->moveablePos[i][j] == true) {
                    printf("++");
                }
                else {
                    printf("**");
                }
            }
            else {
                if (board->block[i][j] == board->chessChoose) color_printf(BLUE_TEXT, "%s", chessName(board->block[i][j]));
                else autoColor_printf(board->block[i][j]->owner, "%s", chessName(board->block[i][j]));
            }
            putchar(' ');
        }
        putchar('\n');
    }

    // 打印玩家击败的棋子
    for (int i = 0; i < 2; i++)
    {
        printf("%s\t", c2tUser(board, i));
        for (int j = 0; j <= board->dead_chess[1-i]->top; j++)
        {
            autoColor_printf(1-i, "%s ", chessName(board->dead_chess[1-i]->stack[j]));
        }
        putchar('\n');
    }
    
    // 提示当前操作玩家及其颜色
    printf("当前操作玩家：");
    autoColor_printf(board->user, "%s", c2tUser(board, board->user));
    fillScreenWithBlank();

    // 提示玩家当前选中棋子
    printf("当前选中棋子：");
    if (board->chessChoose == NULL) printf("未选中棋子");
    else autoColor_printf(board->chessChoose->owner, "%s", chessName(board->chessChoose));
    fillScreenWithBlank();
    if (board->tip->top == -1) {
        fillScreenWithBlank();
        fillScreenWithBlank();
    }

    // 提示操作结果或者错误
    for (int i = 0; i <= board->tip->top; i++)
    {
        printf("%s", board->tip->strs[board->tip->top]);
        fillScreenWithBlank();
    }
    board->tip->top = -1;
    Sleep(NORMAL_INTERVAL);
}