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
   int  i;
    for (i = 9; i >= 0; --i)
    {
        free(matrix[i]);
    }
    free(matrix);
}

struct ChessBoard* initChessBoard() {
    struct ChessBoard *board = (struct ChessBoard *)malloc(sizeof(struct ChessBoard));
    board->objects = initChessStack(32);

     
    int i,j;
	for (i = 4; i >= 0; --i)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            placeChess(board, BOARD_ROW - i - 1, j, initChess(half_board[i][j], PLAYER_1, true));
        }
    }

     
    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            placeChess(board, i, j, initChess(half_board[i][j], PLAYER_2, true));
        }
    }
    
     
    board->moveablePos = initMatrix();

    board->user = PLAYER_1;  
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
    COORD coordScreen = { 0, 0 };     
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    

    
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





void color(int x) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
}

int color_printf(int text_color, const char *format, ...) {
    color(text_color);
    va_list args;           
    va_start(args, format);  
    int len = vprintf(format, args);   
    va_end(args);            
    color(WHITE_TEXT);
    return len;
}

int autoColor_printf(int user, const char *format, ...) {
    int text_color = WHITE_TEXT;
    if (user == PLAYER_1) text_color = RED_TEXT;
    if (user == PLAYER_2) text_color = GREEN_TEXT;
    color(text_color);
    va_list args;            
    va_start(args, format);  
    int len = vprintf(format, args);   
    va_end(args);            
    color(WHITE_TEXT);
    return len;
}

void printChessBoard(struct ChessBoard* board) {
    // cls;
    clearScreen();  
    int i,len,j;
    printf("\n=================================================================\n");
    printf("当前棋盘上的棋子：\n");
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

 
    for (i = 0; i < 2; i++)
    {
        printf("%s\t", c2tUser(board, i));
        for (j = 0; j <= board->dead_chess[1-i]->top; j++)
        {
            autoColor_printf(1-i, "%s ", chessName(board->dead_chess[1-i]->stack[j]));
        }
        putchar('\n');
    }
    
     
    printf("当前操作玩家");
    autoColor_printf(board->user, "%s", c2tUser(board, board->user));
    fillScreenWithBlank();

     
    printf("当前选中棋子");
    if (board->chessChoose == NULL) printf("未选中棋子");
    else autoColor_printf(board->chessChoose->owner, "%s", chessName(board->chessChoose));
    fillScreenWithBlank();
    if (board->tip->top == -1) {
        fillScreenWithBlank();
        fillScreenWithBlank();
    }

     
    for (i = 0; i <= board->tip->top; i++)
    {
        printf("%s", board->tip->strs[board->tip->top]);
        fillScreenWithBlank();
    }
    board->tip->top = -1;
    Sleep(NORMAL_INTERVAL);
}
  
  
  
  
  
  
bool isInside(int row, int col) {
    return (row < BOARD_ROW) && (col < BOARD_COL) && (row >= 0) && (col >= 0);
}

 
 
 
 
 
 
bool isNull(struct ChessBoard* board, int row, int col) {
    return board->block[row][col] == NULL;
}

 
 
 
 
 
bool isControllable(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) && (board->block[row][col]->owner == board->user);
}

 
 
 
 
 
 
 
bool friendlyFireDetect(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) 
    && (board->block[row][col]->owner == board->user);
}

 
 
 
 
 
 
 
 
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
    case GENERAL:  
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
    case CHARIOT:  
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
    case HORSE:  
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
                     
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                }
                if (i == 1)
                {
                     
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                }
                if (i == 2)
                {
                     
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
                if (i == 3)
                {
                     
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
            }
        }

        break;
    }
    case ARTILLERY:
         
         
         
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
         
        char stop[4] = {false, false, false, false};
        for (i = 1; i <= 2; i++)
        {
             
            int
                dest_row[4] = {src_row + i, src_row + i, src_row - i, src_row - i},
                dest_col[4] = {src_col + i, src_col - i, src_col - i, src_col + i};
            for (j = 0; j < 4; j++)
            {
                 
                 
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
             
             
            setBoolMatrix(matrix, src_row, src_col + 1, true);
            setBoolMatrix(matrix, src_row, src_col - 1, true);
        }
         
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

 
 
 
 
bool isMoveable(struct ChessBoard* board, int src_row, int src_col, int dest_row, int dest_col) {
    return board->moveablePos[dest_row][dest_col] == true;
}

 
 
 
 
 
 
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
}

 
 
 
 
 
 
 
 
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
    for (i = 0; i < BOARD_ROW; i++)
    {
        for (j = 0; j < BOARD_COL; j++)
        {
            if (!isNull(board, i, j) && (board->block[i][j]->owner != board->user)) // 闈炵┖涓斾负鏁屼汉
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
    if (strcmp(src, "q") == 0)
    {
        cls;
        for (i = 0; i < 5; i++)
        {
            printf("正在构建主界面");
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

     
    if (strcmp(src, "w") == 0)
    {
        cls;
        printf("不喜欢刚才的决定？\n");
        Sleep(INTERVAL);
        printf("悔棋中。。。\n");
        Sleep(INTERVAL);
        printf("悔棋成功。\n");
        withdraw(board);
        withdraw(board);
        Sleep(INTERVAL);
        return CONTINUE;
    }

     
    int
        src_row = src[0] - '0',
        src_col = src[1] - '0';

    if (choose(board, src_row, src_col) == false)
        return CONTINUE;  

     
    int dest_row, dest_col, action_res;
    do
    {
        action_res = false;
        printChessBoard(board);
        scanf("%s", dest);

         
        if (strcmp(dest, "cancel") == 0)
        {
            actionFinished(board);
            return CONTINUE;
        }
        dest_row = dest[0] - '0';
        dest_col = dest[1] - '0';
        action_res = move(board, src_row, src_col, dest_row, dest_col);  
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
     
    Board board;
    board = initChessBoard();
    while (true)
    {
         
        printChessBoard(board);
        bool action_res;

        bool isBreak = userControl(board, &action_res);
        if (isBreak == BREAK) break;
        if (isBreak == CONTINUE) continue;

         
        printChessBoard(board);
        actionFinished(board);
        if (action_res == true)
        {
            
            if (isGameEnd(board) == true)
            {
                printChessBoard(board);
                putchar('\n');
                printf("goodgame");
                Sleep(NORMAL_INTERVAL);
                break;
            }
            else
                board->user = !board->user;
        }
    }
     
}
char *c2tUser(struct ChessBoard* board, int code) {
    if(code == PLAYER_1) return"玩家一";
    if(code == PLAYER_2) return"玩家二";
    return "";
}

    
    
    
    
    
    
    
char *chessName(struct Chess* chess) {
    switch(chess->owner) {
         
        case PLAYER_1: {
            switch(chess->type) {
                case GENERAL: return "帅";
                case CHARIOT: return "车";
                case HORSE: return "马";
                case ARTILLERY: return "炮";
                case ELEPHANT: return "相";
                case WARRIOR: return "士";
                case SOLDIER: return "兵";
            }
            break;
        }

         
        case PLAYER_2: {
            switch(chess->type) {
                case GENERAL: return "将";
                case CHARIOT: return "俥";
                case HORSE: return "傌";
                case ARTILLERY: return "炮";
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
    int i;
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
                    sprintf(board->tip->strs[++board->tip->top], "%s的%s已经被击败", c2tUser(board, dead->owner), chessName(dead));
                    ChessStackPush(cstk, dead);
                }
                int cmd[] = {src_row, src_col, dest_row, dest_col};
                ++(board->record.top);
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
    struct Chess *dest_block = cmd[RECORD_CMD_LEN - 1];
    setChessBoardBlock(board, dest_row, dest_col, dest_block);
    if (dest_block != NULL)
        board->dead_chess[dest_block->owner]->top--;
    return true;
}
