#include "global.h"
#include "chess.h"

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
