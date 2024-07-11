#include "global.h"
#include "chess_board.h"

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
