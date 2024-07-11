#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "global.h"
#include "chess.h"
#include "chess_stack.h"

struct Tips {
    char strs[8][64];
    int top;
};

struct RecordStack {
    int **commands;
    int top;
};

typedef struct ChessBoard
{
    struct Chess *block[10][9];
    struct ChessStack *objects;
    char **moveablePos;
    PLAYER user;
    struct Chess *chessChoose;
    struct ChessStack **dead_chess;
    struct Tips *tip;
    struct RecordStack record;
} *Board;

char **initMatrix();
void freeMatrix(char **matrix);
struct ChessBoard* initChessBoard();
bool setChessBoardBlock(struct ChessBoard *board, int row, int col, struct Chess *chess);
void reset_moveablePos(struct ChessBoard *board);
int setChessBoardMoveablePos(struct ChessBoard *board, int row, int col, int val);
bool placeChess(struct ChessBoard *board, int row, int col, struct Chess *chess);
void aiLogic(struct ChessBoard *board, int *dest_row, int *dest_col);

#endif