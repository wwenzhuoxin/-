#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "chess_board.h"

/*
    1.不可移动：
        1.1. 原位置不存在己方棋子或不存在棋子
        1.2. 目标位置超出棋盘范围
        1.3. 目标位置存在己方棋子
        1.4. 不符合棋子移动规则
    2. 可移动：操纵了己方棋子，未超出棋盘范围，且符合移动规则，且目标位置不存在己方棋子
        2.1. 击杀
        2.2. 无击杀
*/

int choose(struct ChessBoard* board, int src_row, int src_col);
int move(struct ChessBoard* board, int src_row, int src_col, int dest_row, int dest_col);
bool withdraw(struct ChessBoard *board);

#endif