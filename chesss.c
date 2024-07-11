#include "global.h"
#include "chess.h"
#include "chess_board.h"
#include "chess_io.h"
#include "tools.h"
#include "chess_stack.h"
#include "new_game.h"
#include "chess_logic.h"
#include "operations.h"
#define INTERVAL SHORT_INTERVAL
#define BREAK 100
#define CONTINUE 101
int count = 0;

struct Chess *initChess(int type, int owner, int is_alive) {
    if (type == EMPTY)
        return NULL;
    struct Chess* chess = (struct Chess*) malloc(sizeof(struct Chess));
    chess->id = count++;
    chess->type = type;
    chess->owner = owner;
    chess->is_alive = is_alive;
    return chess;
}

struct Position* initPosition(int x, int y) {
    struct Position* pos = (struct Position*) malloc(sizeof(struct Position));
    pos->x = x;
    pos->y = y;
    return pos;
}
int half_board[5][9] = 
{
    {CHARIOT, HORSE, ELEPHANT, WARRIOR, GENERAL, WARRIOR, ELEPHANT, HORSE, CHARIOT},    // 0
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,},                   // 1
    {EMPTY, ARTILLERY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, ARTILLERY, EMPTY},     // 2
    {SOLDIER, EMPTY, SOLDIER, EMPTY, SOLDIER, EMPTY, SOLDIER, EMPTY, SOLDIER},          // 3
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,},                   // 4
};

char** initMatrix() {
    char **matrix = (char **)malloc(sizeof(char *) * BOARD_ROW);
    int i,j; 
	for (i = 0; i < 10; i++)
    {
        matrix[i] = (char *) malloc(sizeof(char) * BOARD_COL);
        for (j = 0; j < 9; j++)
        {
            matrix[i][j] = false;
        } 
    }
    return matrix;
}

void freeMatrix(char **matrix) {
   int i;
    for (i = 9; i >= 0; --i)
    {
        free(matrix[i]);
    }
    free(matrix);
}

struct ChessBoard* initChessBoard() {
    struct ChessBoard *board = (struct ChessBoard *)malloc(sizeof(struct ChessBoard));
    board->objects = initChessStack(32);

    // 玩家一初始化
    int i,j;
	for (i = 4; i >= 0; --i)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            placeChess(board, BOARD_ROW - i - 1, j, initChess(half_board[i][j], PLAYER_1, true));
        }
    }

    // 玩家二初始化
    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            placeChess(board, i, j, initChess(half_board[i][j], PLAYER_2, true));
        }
    }
    
    // 可移动区域初始化
    board->moveablePos = initMatrix();

    board->user = PLAYER_1; // 开局玩家初始化
    board->chessChoose = NULL;
    board->dead_chess = (struct ChessStack **)malloc(sizeof(struct ChessStack *) * 2);
    board->dead_chess[PLAYER_1] = initChessStack_default();
    board->dead_chess[PLAYER_2] = initChessStack_default();
    board->tip = (struct Tips *)malloc(sizeof(struct Tips));
    board->tip->top = -1;
    board->record.commands = (int **)malloc(sizeof(int *) * 200);
	for (i = 0; i < 200; i++)
    {
        board->record.commands[i] = (int *)malloc(sizeof(int) * RECORD_CMD_LEN);
    }
    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            board->record.commands[i][j] = NULL_CMD;
        }
    }
    board->record.top = -1;
    return board;
}

bool setChessBoardBlock(struct ChessBoard *board, int row, int col, struct Chess *chess) {
    if ((0 <= row && row <= 9) && (0 <= col && col <= 8)) {
        board->block[row][col] = chess;
        if (chess != NULL) {
            chess->pos.x = row;
            chess->pos.y = col;
        }
        return true;
    }
    else return false;
}

void reset_moveablePos(struct ChessBoard *board) {
    int i,j;
	for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 9; j++)
        {
            board->moveablePos[i][j] = false;
        } 
    }
}

int setChessBoardMoveablePos(struct ChessBoard *board, int row, int col, int val) {
    if ((0 <= row && row <= 9) && (0 <= col && col <= 8)) {
        board->moveablePos[row][col] = val;
        return true;
    }
    else return false;
}

bool placeChess(struct ChessBoard *board, int row, int col, struct Chess *chess) {
    if (chess != NULL) {
        ChessStackPush(board->objects, chess);
    }
    return setChessBoardBlock(board, row, col, chess);
}
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
   int i;
    for (i = 0; i < 50; i++)
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
    int i,len,j;
	for (i = 0, len = 0; i <= board->objects->top; i++, len++)
    {
        if (len >= 8) {
            len = 0;
            putchar('\n');
        }
        autoColor_printf(board->objects->stack[i]->owner, "%s(%d)\t", chessName(board->objects->stack[i]), i);
    }
    printf("\n=================================================================\n");

    printf("  ");
    for (i = 0; i < 9; i++)
    {
        printf("%d  ", i);
    }
    putchar('\n');  
    
    for (i = 0; i < BOARD_ROW; i++)
    {
        printf("%d ", i);
        for (j = 0; j < BOARD_COL; j++)
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
    for (i = 0; i < 2; i++)
    {
        printf("%s\t", c2tUser(board, i));
        for (j = 0; j <= board->dead_chess[1-i]->top; j++)
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
    for (i = 0; i <= board->tip->top; i++)
    {
        printf("%s", board->tip->strs[board->tip->top]);
        fillScreenWithBlank();
    }
    board->tip->top = -1;
    Sleep(NORMAL_INTERVAL);
}
/**
 * @brief 是否超出棋盘范围
 * 
 * @param row 目标行
 * @param col 目标列
 * @return bool 返回false，若目标位置超出棋盘；否则，返回true
 */
bool isInside(int row, int col) {
    return (row < BOARD_ROW) && (col < BOARD_COL) && (row >= 0) && (col >= 0);
}

/**
 * @brief 当前坐标位置是否存在棋子
 * 
 * @param board 棋盘
 * @param row 行
 * @param col 列
 * @return bool 返回true，若不存在棋子；返回false，若存在棋子
 */
bool isNull(struct ChessBoard* board, int row, int col) {
    return board->block[row][col] == NULL;
}

/**
 * @brief 原位置是否存在己方棋子
 * 
 * @param board 棋盘
 * @param row 原行
 * @param col 原列
 * @return bool 返回true，若存在己方棋子；返回false，若不存在棋子或存在敌方棋子
 */
bool isControllable(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) && (board->block[row][col]->owner == board->user);
}

/**
 * @brief 目标位置是否存在己方棋子，是否会造成友伤
 * 
 * @param board 棋盘
 * @param row 目标行
 * @param col 目标列
 * @return bool 返回true，若目标位置为我方棋子；返回false，若目标位置为空或敌方棋子
 */
bool friendlyFireDetect(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) 
    && (board->block[row][col]->owner == board->user);
}

/**
 * @brief 位置是否在玩家领地内；玩家一的领地是5-9行，玩家二的领地是0-4行
 * 使用前最好先使用isInside()
 * 
 * @param board 棋盘
 * @param row 
 * @param col 
 * @return bool 返回true，若玩家不在自己的领地内；否则返回false
 */
bool isBeyond(struct ChessBoard* board, int row, int col) {
    if (board->user == PLAYER_1) 
        return !((5 <= row && row <= 9) && (0 <= col && col <= 8));
    if (board->user == PLAYER_2) 
        return !((0 <= row && row <= 4) && (0 <= col && col <= 8));
    return true;
}

bool isInsidePalace(struct ChessBoard* board, int row, int col) {
    if (board->user == PLAYER_1) 
        return (7 <= row) && (row <= 9) && (3 <= col) && (col <= 5);
    if (board->user == PLAYER_2) 
        return (0 <= row) && (row <= 2) && (3 <= col) && (col <= 5);
    return true;
}

bool setBoolMatrix(char **matrix, int row, int col, char value) {
    if ((0 <= row && row <= 9) && (0 <= col && col <= 8)) {
        matrix[row][col] = value;
        return true;
    }
    else return false;
}

char** my_pathFindingByChess(struct ChessBoard *board, struct Chess* chess) {
    int src_row = chess->pos.x, src_col = chess->pos.y;
    char **matrix = initMatrix();
    int i,j;
	switch (chess->type)
    {
    case GENERAL: // 是否在九宫格内且移动了一格
    {
        int dest_row[4], dest_col[4];
        dest_row[0] = src_row + 1;
        dest_row[1] = src_row;
        dest_row[2] = src_row - 1;
        dest_row[3] = src_row;
        dest_col[0] = src_col;
        dest_col[1] = src_col + 1;
        dest_col[2] = src_col;
        dest_col[3] = src_col - 1;
		for (i = 0; i < 4; i++)
        {
            if (isInsidePalace(board, dest_row[i], dest_col[i]) == true)
                matrix[dest_row[i]][dest_col[i]] = true;
        }
        break;
    }
    case CHARIOT: // 车，目标位置与源位置是否在同一行或同一列
    {
		for (i = src_row + 1, j = src_col; isInside(i, j); ++i)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (i = src_row - 1, j = src_col; isInside(i, j); --i)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (i = src_row, j = src_col + 1; isInside(i, j); ++j)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (i = src_row, j = src_col - 1; isInside(i, j); --j)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        break;
    }
    case HORSE: // 马
    {
        int dest_row[4], dest_col[4];
        dest_row[0] = src_row + 1;
        dest_row[1] = src_row;
        dest_row[2] = src_row - 1;
        dest_row[3] = src_row;
        dest_col[0] = src_col;
        dest_col[1] = src_col + 1;
        dest_col[2] = src_col;
        dest_col[3] = src_col - 1;
        for (i = 0; i < 4; i++)
        {
            if (isInside(dest_row[i], dest_col[i]) && isNull(board, dest_row[i], dest_col[i]))
            {
                if (i == 0)
                {
                    // 下
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                }
                if (i == 1)
                {
                    // 右
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                }
                if (i == 2)
                {
                    // 上
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
                if (i == 3)
                {
                    // 左
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
            }
        }

        break;
    }
    case ARTILLERY:
        // 目标位置与源位置是否在同一行或同一列
        // 且目标位置有敌人，目标位置与源位置之间是否有棋子
        {
            int i, j;
            for (i = src_row + 1, j = src_col; isInside(i, j) && isNull(board, i, j); ++i)
            {
                matrix[i][j] = true;
            }
            for (++i; isInside(i, j); ++i)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row - 1, j = src_col; isInside(i, j) && isNull(board, i, j); --i)
            {
                matrix[i][j] = true;
            }
            for (--i; isInside(i, j); --i)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row, j = src_col + 1; isInside(i, j) && isNull(board, i, j); ++j)
            {
                matrix[i][j] = true;
            }
            for (++j; isInside(i, j); ++j)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row, j = src_col - 1; isInside(i, j) && isNull(board, i, j); --j)
            {
                matrix[i][j] = true;
            }
            for (--j; isInside(i, j); --j)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            break;
        }
    case ELEPHANT:
    {
        // 在内部且路径上没有敌人
        char stop[4] = {false, false, false, false};
        for (i = 1; i <= 2; i++)
        {
            // 右下角，左下角，左上角，右上角
            int
                dest_row[4] = {src_row + i, src_row + i, src_row - i, src_row - i},
                dest_col[4] = {src_col + i, src_col - i, src_col - i, src_col + i};
            for (j = 0; j < 4; j++)
            {
                // 如果位置不在棋盘内、越界或者有棋子阻挡象眼，则停止前进预测
                // 否则就认为可前进
                if (i == 1 && (isBeyond(board, dest_row[j], dest_col[j]) || !isNull(board, dest_row[j], dest_col[j])))
                    stop[j] = true;
                if (i == 2 && stop[j] == false)
                {
                    setBoolMatrix(matrix, dest_row[j], dest_col[j], true);
                }
            }
        }
        break;
    }
    case WARRIOR:
    {
        // 右下角，左下角，左上角，右上角
        int
            dest_row[4] = {src_row + 1, src_row + 1, src_row - 1, src_row - 1},
            dest_col[4] = {src_col + 1, src_col - 1, src_col - 1, src_col + 1};
        for (i = 0; i < 4; i++)
        {
            if (isInsidePalace(board, dest_row[i], dest_col[i]))
            {
                setBoolMatrix(matrix, dest_row[i], dest_col[i], true);
            }
        }
        break;
    }
    case SOLDIER:
    {
        if (isInside(src_row, src_col) && isBeyond(board, src_row, src_col))
        {
            // 越过领地（楚河汉界），可以向右左走
            setBoolMatrix(matrix, src_row, src_col + 1, true);
            setBoolMatrix(matrix, src_row, src_col - 1, true);
        }
        // 无论何时都可以向前走（当前也没法向后走了
        if (board->user == PLAYER_1)
            setBoolMatrix(matrix, src_row - 1, src_col, true);
        if (board->user == PLAYER_2)
            setBoolMatrix(matrix, src_row + 1, src_col, true);
        break;
    }
    }
    return matrix;
}

char** my_pathFindingByPos(struct ChessBoard *board, int src_row, int src_col) {
    struct Chess* chess = board->block[src_row][src_col];
    return my_pathFindingByChess(board, chess);
}

void moveablePosition(struct ChessBoard* board, int src_row, int src_col) {
    reset_moveablePos(board);
    freeMatrix(board->moveablePos);
    board->moveablePos = my_pathFindingByPos(board, src_row, src_col);
}

// 是否符合棋子移动规则
bool isMoveable(struct ChessBoard* board, int src_row, int src_col, int dest_row, int dest_col) {
    return board->moveablePos[dest_row][dest_col] == true;
}

/**
 * @brief 判断游戏是否结束（是否将死）
 * 
 * @param board 棋盘
 * @return bool 返回true，如果已经将死；否则返回false
 */
bool isGameEnd(struct ChessBoard* board) {
    int i;
	for (i = 0; i < 2; i++)
    {
        if ((isChessStackEmpty(board->dead_chess[i]) == false) && (ChessStackTop(board->dead_chess[i])->type == GENERAL))
        {
            return true;
        }
    }
    return false;
    // if (
    // (isChessStackEmpty(board->dead_player1) == false && ChessStackTop(board->dead_player1)->type == GENERAL) || 
    // (isChessStackEmpty(board->dead_player2) == false && ChessStackTop(board->dead_player2)->type == GENERAL))
    //     return true;
    // else return false;
}

/**
 * @brief 行动完成，可移动路径图层和已选择棋子清空
 * 
 * @param board 棋盘
 */
void actionFinished(struct ChessBoard *board) {
    int i,j;
	for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 9; j++)
        {
            board->moveablePos[i][j] = false;
        } 
    }
    board->chessChoose = NULL;
}

void aiLogic(struct ChessBoard *board, int *dest_row, int *dest_col) {
    ChessPtr chess;
    int i,j;
	for (i = 0; i <= board->objects->top; i++)
    {
        if (board->objects->stack[i]->owner != board->user) {
            chess = board->objects->stack[i];
            my_pathFindingByChess(board, chess);
            break;
        }
    }
    for ( i = 0; i < BOARD_ROW; i++)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            if (!isNull(board, i, j) && (board->block[i][j]->owner != board->user)) // 非空且为敌人
            {
            }
        }
    }
    
}
struct ChessStack* initChessStack(int max_size) {
    struct ChessStack* stack = (struct ChessStack*) malloc(sizeof(struct ChessStack));
    stack->stack = (struct Chess**) malloc(sizeof(struct Chess*) * max_size);
    stack->top = -1;
    stack->maxSize = max_size - 1;
    return stack;
}

struct ChessStack* initChessStack_default() {
    return initChessStack(16);
}

/**
 * @brief 判断栈是否为空
 * 
 * @param stack 棋子的栈
 * @return int 返回true，如果栈为空；返回false，若果栈非空
 */
int isChessStackEmpty(struct ChessStack *chess_stk) {
    if (chess_stk->top == -1) return true;
    else return false;
}

int ChessStackPush(struct ChessStack *chess_stk, struct Chess *chess) {
    if (chess_stk->top < chess_stk->maxSize) {
        chess_stk->stack[++chess_stk->top] = chess;
        return true;
    }
    else return false;
}

struct Chess *ChessStackTop(struct ChessStack *chess_stk) {
    if (isChessStackEmpty(chess_stk) == true) return NULL;
    else return chess_stk->stack[chess_stk->top];
}
int main(int argc, char const *argv[])
{
    initChessGame();

    return 0;
}
int userControl(struct ChessBoard *board, bool *res)
{
    char src[10], dest[10];
    scanf("%s", src);
    int i;
    // 退出
    if (strcmp(src, "q") == 0)
    {
        cls;
        for (i = 0; i < 5; i++)
        {
            printf("伊蕾娜正在构建主界面中");
            for (i = 0; i < 3; i++)
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
char *c2tUser(struct ChessBoard* board, int code) {
    if(code == PLAYER_1) return"���һ";
    if(code == PLAYER_2) return"��Ҷ�";
    return "";
}

/**
 * @brief 输入棋子，输出棋子中文名字，区分玩家
 * 
 * @param chess 玩家棋子
 * @return char* 棋子中文名字
 */
char *chessName(struct Chess* chess) {
    switch(chess->owner) {
        // 玩家一是红色
        case PLAYER_1: {
            switch(chess->type) {
                case GENERAL: return "帥";
                case CHARIOT: return "俥";
                case HORSE: return "傌";
                case ARTILLERY: return "炮";
                case ELEPHANT: return "相";
                case WARRIOR: return "仕";
                case SOLDIER: return "兵";
            }
            break;
        }

        // 玩家二是绿色
        case PLAYER_2: {
            switch(chess->type) {
                case GENERAL: return "将";
                case CHARIOT: return "車";
                case HORSE: return "馬";
                case ARTILLERY: return "砲";
                case ELEPHANT: return "象";
                case WARRIOR: return "士";
                case SOLDIER: return "卒";
            }
            break;
        }
    }

    return " ";
}
int choose(struct ChessBoard *board, int src_row, int src_col)
{
    if (isControllable(board, src_row, src_col) == false)
        return false;
    else
    {
        board->chessChoose = board->block[src_row][src_col];
        moveablePosition(board, src_row, src_col);
        return true;
    }
}

int move(struct ChessBoard *board, int src_row, int src_col, int dest_row, int dest_col)
{
    
	if (isInside(dest_row, dest_col))
    {
        if (!friendlyFireDetect(board, dest_row, dest_col))
        {
            if (isMoveable(board, src_row, src_col, dest_row, dest_col))
            {
                bool isKill = !isNull(board, dest_row, dest_col) && (board->block[dest_row][dest_col]->owner != board->user);
                struct ChessStack *cstk;
                struct Chess *dead = NULL;
                if (isKill == true)
                {
                    dead = board->block[dest_row][dest_col];
                    dead->is_alive = false;
                    cstk = board->dead_chess[board->block[dest_row][dest_col]->owner];
                    sprintf(board->tip->strs[++board->tip->top], "%s的%s已经被击败了", c2tUser(board, dead->owner), chessName(dead));
                    ChessStackPush(cstk, dead);
                }
                int cmd[] = {src_row, src_col, dest_row, dest_col};
                ++(board->record.top);
                int i;
				for (i = 0; i < RECORD_CMD_LEN - 1; i++)
                {
                    board->record.commands[board->record.top][i] = cmd[i];
                }
                board->record.commands[board->record.top][RECORD_CMD_LEN - 1] = (long long int) dead;
                setChessBoardBlock(board, dest_row, dest_col, board->block[src_row][src_col]);
                setChessBoardBlock(board, src_row, src_col, NULL);
                return true;
            }
        }
    }
    sprintf(board->tip->strs[++board->tip->top], "不可移动到这里");
    return false;
}

bool withdraw(struct ChessBoard *board)
{
    if (board->record.top == -1)
        return false;
    int *cmd = board->record.commands[board->record.top--];
    int src_row = cmd[0], src_col = cmd[1], dest_row = cmd[2], dest_col = cmd[3];

    struct Chess *src_block = board->block[dest_row][dest_col];
    setChessBoardBlock(board, src_row, src_col, src_block);
    struct Chess *dest_block =(unsigned int *) cmd[RECORD_CMD_LEN - 1];
    setChessBoardBlock(board, dest_row, dest_col, dest_block);
    if (dest_block != NULL)
        board->dead_chess[dest_block->owner]->top--;
    return true;
}
