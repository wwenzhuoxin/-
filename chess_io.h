#ifndef CHESS_IO_H
#define CHESS_IO_H

#include "chess_board.h"

#define cls system("cls")

typedef struct PlayerCommand {
    int command_type;
    short int args[4];
} *Cmd;

Cmd init_player_command(int command_type, ...);

static bool input_status = 0;

void clearScreen();
void color(int x);
int autoColor_printf(int user, const char *format, ...);
int color_printf(int text_color, const char *format, ...);
void printChessBoard(struct ChessBoard* board);

#endif