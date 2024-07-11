#include "global.h"
#include "operations.h"
#include "chess_logic.h"
#include "tools.h"

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

/**
 * @brief 选择棋子，若可以操纵，则记录当前选中棋子，计算出可移动路径图层
 *
 * @param board 棋盘
 * @param src_row
 * @param src_col
 * @return int 返回true，若棋子可以操纵；否则，返回false
 */
int choose(struct ChessBoard *board, int src_row, int src_col)
{
    if (isControllable(board, src_row, src_col) == false)
        return false;
    else
    {
        board->chessChoose = board->block[src_row][src_col];
        moveablePosition(board, src_row, src_col);
        return true;
    }
}

int move(struct ChessBoard *board, int src_row, int src_col, int dest_row, int dest_col)
{
    if (isInside(dest_row, dest_col))
    {
        if (!friendlyFireDetect(board, dest_row, dest_col))
        {
            if (isMoveable(board, src_row, src_col, dest_row, dest_col))
            {
                bool isKill = !isNull(board, dest_row, dest_col) && (board->block[dest_row][dest_col]->owner != board->user);
                struct ChessStack *cstk;
                struct Chess *dead = NULL;
                if (isKill == true)
                {
                    dead = board->block[dest_row][dest_col];
                    dead->is_alive = false;
                    cstk = board->dead_chess[board->block[dest_row][dest_col]->owner];
                    sprintf(board->tip->strs[++board->tip->top], "%s的%s已经被击败了", c2tUser(board, dead->owner), chessName(dead));
                    ChessStackPush(cstk, dead);
                }
                int cmd[] = {src_row, src_col, dest_row, dest_col};
                ++(board->record.top);
                for (int i = 0; i < RECORD_CMD_LEN - 1; i++)
                {
                    board->record.commands[board->record.top][i] = cmd[i];
                }
                board->record.commands[board->record.top][RECORD_CMD_LEN - 1] = (int) dead;
                setChessBoardBlock(board, dest_row, dest_col, board->block[src_row][src_col]);
                setChessBoardBlock(board, src_row, src_col, NULL);
                return true;
            }
        }
    }
    sprintf(board->tip->strs[++board->tip->top], "�����ƶ�������");
    return false;
}

bool withdraw(struct ChessBoard *board)
{
    if (board->record.top == -1)
        return false;
    int *cmd = board->record.commands[board->record.top--];
    int src_row = cmd[0], src_col = cmd[1], dest_row = cmd[2], dest_col = cmd[3];

    struct Chess *src_block = board->block[dest_row][dest_col];
    setChessBoardBlock(board, src_row, src_col, src_block);
    struct Chess *dest_block = cmd[RECORD_CMD_LEN - 1];
    setChessBoardBlock(board, dest_row, dest_col, dest_block);
    if (dest_block != NULL)
        board->dead_chess[dest_block->owner]->top--;
    return true;
}
