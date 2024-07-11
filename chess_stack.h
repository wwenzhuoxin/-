#ifndef CHESS_STACK_H
#define CHESS_STACK_H

struct ChessStack;

struct ChessStack
{
    struct Chess **stack;
    int top;
    int maxSize;
};

struct ChessStack* initChessStack(int maxSize);
struct ChessStack* initChessStack_default();
int ChessStackPush(struct ChessStack *stack, struct Chess *chess);
int isChessStackEmpty(struct ChessStack *stack);
struct Chess *ChessStackTop(struct ChessStack *stack);

#endif