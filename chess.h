#ifndef CHESS_H
#define CHESS_H

typedef int PLAYER;

struct Position
{
    int x; // 横坐标
    int y; // 纵坐标
};

typedef struct Chess
{
    int id;
    int type;
    PLAYER owner; // 
    char is_alive;
    struct Position pos;
} *ChessPtr;

struct Chess *initChess(int type, int owner, int is_alive);
struct Position* initPosition(int x, int y);

#endif