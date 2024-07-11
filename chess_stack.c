#include "global.h"
#include "chess_stack.h"

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