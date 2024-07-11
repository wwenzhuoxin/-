#include "global.h"
#include "new_game.h"
#include "chess_io.h"
#include "chess_logic.h"
#include "operations.h"

// #define INTERVAL NORMAL_INTERVAL
#define INTERVAL SHORT_INTERVAL
#define BREAK 100
#define CONTINUE 101

/**
 * 游戏可以分为几个阶段：
 * 1. 第一阶段：
 *      1.1. 选中 象棋 —— 第二阶段
 *      1.2. 回退 操作/指令 —— 继续循环
 *      1.3. 退出 —— 结束循环
 * 2. 第二阶段：
 *      2.1. 移动 象棋 —— 继续循环
 *      2.2. 撤销选中 —— 继续循环
 */

int userControl(struct ChessBoard *board, bool *res)
{
    char src[10], dest[10];
    scanf("%s", src);

    // 退出
    if (strcmp(src, "q") == 0)
    {
        cls;
        for (int i = 0; i < 5; i++)
        {
            printf("伊蕾娜正在构建主界面中");
            for (int i = 0; i < 3; i++)
            {
                putchar('.');
                Sleep(500);
            }
            cls;
        }
        cls;
        Sleep(INTERVAL);
        return BREAK;
    }

    // 撤回 
    if (strcmp(src, "w") == 0)
    {
        cls;
        printf("伊蕾娜不喜欢刚才做的决定...\n");
        Sleep(INTERVAL);
        printf("天才美少女魔女伊蕾娜正在使用魔法...\n");
        Sleep(INTERVAL);
        printf("败者食尘！\n");
        withdraw(board);
        withdraw(board);
        Sleep(INTERVAL);
        return CONTINUE;
    }

    // 选中
    int
        src_row = src[0] - '0',
        src_col = src[1] - '0';

    if (choose(board, src_row, src_col) == false)
        return CONTINUE; // 一旦执行了该函数，必须在下一循环前使用actionFinished函数

    // 第二阶段
    int dest_row, dest_col, action_res;
    do
    {
        action_res = false;
        printChessBoard(board);
        scanf("%s", dest);

        // 取消选中
        if (strcmp(dest, "cancel") == 0)
        {
            actionFinished(board);
            return CONTINUE;
        }
        dest_row = dest[0] - '0';
        dest_col = dest[1] - '0';
        action_res = move(board, src_row, src_col, dest_row, dest_col); // 移动
    } while (action_res == false);
    *res = action_res;
}

int aiControl(struct ChessBoard *board, bool *res)
{
    int dest_row, dest_col;
    aiLogic(board, &dest_row, &dest_col);
}

void initChessGame()
{
    // 初始化棋盘
    Board board;
    board = initChessBoard();
    while (true)
    {
        // 待选择阶段
        printChessBoard(board);
        bool action_res;

        bool isBreak = userControl(board, &action_res);
        if (isBreak == BREAK) break;
        if (isBreak == CONTINUE) continue;

        // 第三阶段 —— 后移动阶段，不需要输入
        printChessBoard(board);
        actionFinished(board);
        if (action_res == true)
        {
            // 如果玩家已经行动
            if (isGameEnd(board) == true)
            {
                printChessBoard(board);
                putchar('\n');
                printf("游戏结束");
                Sleep(NORMAL_INTERVAL);
                break;
            }
            else
                board->user = !board->user;
        }
    }
    // getchar();
}

