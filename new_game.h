#ifndef NEW_GAME_H
#define NEW_GAME_H

#include "global.h"
#include "chess_board.h"

int userControl(struct ChessBoard *, bool *);
int aiControl(struct ChessBoard *, bool *);
void initChessGame();

#endif