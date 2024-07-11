#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>

#define BOARD_ROW 10
#define BOARD_COL 9
#define RECORD_CMD_LEN 5
#define NULL_CMD -100
#define SHORTER_INTERVAL 100
#define SHORT_INTERVAL 500
#define NORMAL_INTERVAL 2000
#define GREEN_TEXT 2
#define RED_TEXT 4
#define WHITE_TEXT 7
#define BLUE_TEXT 9

// block type
#define EMPTY 0
#define GENERAL 1 // 将/帅
#define CHARIOT 2 // 车
#define HORSE 3 // 马
#define ARTILLERY 4 // 炮
#define ELEPHANT 5 // 象/相
#define WARRIOR 6 // 士/仕
#define SOLDIER 7 // 卒

// owner
#define PLAYER_1 0
#define PLAYER_2 1
#define SPARE 3

// is_alive
#define DEAD false
#define ALIVE true

#endif