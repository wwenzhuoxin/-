#include "global.h"
#include "chess_board.h"

/**
 * @brief 是否超出棋盘范围
 * 
 * @param row 目标行
 * @param col 目标列
 * @return bool 返回false，若目标位置超出棋盘；否则，返回true
 */
bool isInside(int row, int col) {
    return (row < BOARD_ROW) && (col < BOARD_COL) && (row >= 0) && (col >= 0);
}

/**
 * @brief 当前坐标位置是否存在棋子
 * 
 * @param board 棋盘
 * @param row 行
 * @param col 列
 * @return bool 返回true，若不存在棋子；返回false，若存在棋子
 */
bool isNull(struct ChessBoard* board, int row, int col) {
    return board->block[row][col] == NULL;
}

/**
 * @brief 原位置是否存在己方棋子
 * 
 * @param board 棋盘
 * @param row 原行
 * @param col 原列
 * @return bool 返回true，若存在己方棋子；返回false，若不存在棋子或存在敌方棋子
 */
bool isControllable(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) && (board->block[row][col]->owner == board->user);
}

/**
 * @brief 目标位置是否存在己方棋子，是否会造成友伤
 * 
 * @param board 棋盘
 * @param row 目标行
 * @param col 目标列
 * @return bool 返回true，若目标位置为我方棋子；返回false，若目标位置为空或敌方棋子
 */
bool friendlyFireDetect(struct ChessBoard* board, int row, int col) {
    return !isNull(board, row, col) 
    && (board->block[row][col]->owner == board->user);
}

/**
 * @brief 位置是否在玩家领地内；玩家一的领地是5-9行，玩家二的领地是0-4行
 * 使用前最好先使用isInside()
 * 
 * @param board 棋盘
 * @param row 
 * @param col 
 * @return bool 返回true，若玩家不在自己的领地内；否则返回false
 */
bool isBeyond(struct ChessBoard* board, int row, int col) {
    if (board->user == PLAYER_1) 
        return !((5 <= row && row <= 9) && (0 <= col && col <= 8));
    if (board->user == PLAYER_2) 
        return !((0 <= row && row <= 4) && (0 <= col && col <= 8));
    return true;
}

bool isInsidePalace(struct ChessBoard* board, int row, int col) {
    if (board->user == PLAYER_1) 
        return (7 <= row) && (row <= 9) && (3 <= col) && (col <= 5);
    if (board->user == PLAYER_2) 
        return (0 <= row) && (row <= 2) && (3 <= col) && (col <= 5);
    return true;
}

bool setBoolMatrix(char **matrix, int row, int col, char value) {
    if ((0 <= row && row <= 9) && (0 <= col && col <= 8)) {
        matrix[row][col] = value;
        return true;
    }
    else return false;
}

char** pathFindingByChess(struct ChessBoard *board, struct Chess* chess) {
    int src_row = chess->pos.x, src_col = chess->pos.y;
    char **matrix = initMatrix();
    switch (chess->type)
    {
    case GENERAL: // 是否在九宫格内且移动了一格
    {
        int dest_row[4], dest_col[4];
        dest_row[0] = src_row + 1;
        dest_row[1] = src_row;
        dest_row[2] = src_row - 1;
        dest_row[3] = src_row;
        dest_col[0] = src_col;
        dest_col[1] = src_col + 1;
        dest_col[2] = src_col;
        dest_col[3] = src_col - 1;
        for (int i = 0; i < 4; i++)
        {
            if (isInsidePalace(board, dest_row[i], dest_col[i]) == true)
                matrix[dest_row[i]][dest_col[i]] = true;
        }
        break;
    }
    case CHARIOT: // 车，目标位置与源位置是否在同一行或同一列
    {
        for (int i = src_row + 1, j = src_col; isInside(i, j); ++i)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (int i = src_row - 1, j = src_col; isInside(i, j); --i)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (int i = src_row, j = src_col + 1; isInside(i, j); ++j)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        for (int i = src_row, j = src_col - 1; isInside(i, j); --j)
        {
            matrix[i][j] = true;
            if (isNull(board, i, j) == false)
                break;
        }
        break;
    }
    case HORSE: // 马
    {
        int dest_row[4], dest_col[4];
        dest_row[0] = src_row + 1;
        dest_row[1] = src_row;
        dest_row[2] = src_row - 1;
        dest_row[3] = src_row;
        dest_col[0] = src_col;
        dest_col[1] = src_col + 1;
        dest_col[2] = src_col;
        dest_col[3] = src_col - 1;
        for (int i = 0; i < 4; i++)
        {
            if (isInside(dest_row[i], dest_col[i]) && isNull(board, dest_row[i], dest_col[i]))
            {
                if (i == 0)
                {
                    // 下
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                }
                if (i == 1)
                {
                    // 右
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                }
                if (i == 2)
                {
                    // 上
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] + 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
                if (i == 3)
                {
                    // 左
                    setBoolMatrix(matrix, dest_row[i] + 1, dest_col[i] - 1, true);
                    setBoolMatrix(matrix, dest_row[i] - 1, dest_col[i] - 1, true);
                }
            }
        }

        break;
    }
    case ARTILLERY:
        // 目标位置与源位置是否在同一行或同一列
        // 且目标位置有敌人，目标位置与源位置之间是否有棋子
        {
            int i, j;
            for (i = src_row + 1, j = src_col; isInside(i, j) && isNull(board, i, j); ++i)
            {
                matrix[i][j] = true;
            }
            for (++i; isInside(i, j); ++i)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row - 1, j = src_col; isInside(i, j) && isNull(board, i, j); --i)
            {
                matrix[i][j] = true;
            }
            for (--i; isInside(i, j); --i)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row, j = src_col + 1; isInside(i, j) && isNull(board, i, j); ++j)
            {
                matrix[i][j] = true;
            }
            for (++j; isInside(i, j); ++j)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            for (i = src_row, j = src_col - 1; isInside(i, j) && isNull(board, i, j); --j)
            {
                matrix[i][j] = true;
            }
            for (--j; isInside(i, j); --j)
            {
                if (!isNull(board, i, j))
                {
                    if (!friendlyFireDetect(board, i, j))
                        matrix[i][j] = true;
                    break;
                }
            }
            break;
        }
    case ELEPHANT:
    {
        // 在内部且路径上没有敌人
        char stop[4] = {false, false, false, false};
        for (int i = 1; i <= 2; i++)
        {
            // 右下角，左下角，左上角，右上角
            int
                dest_row[4] = {src_row + i, src_row + i, src_row - i, src_row - i},
                dest_col[4] = {src_col + i, src_col - i, src_col - i, src_col + i};
            for (int j = 0; j < 4; j++)
            {
                // 如果位置不在棋盘内、越界或者有棋子阻挡象眼，则停止前进预测
                // 否则就认为可前进
                if (i == 1 && (isBeyond(board, dest_row[j], dest_col[j]) || !isNull(board, dest_row[j], dest_col[j])))
                    stop[j] = true;
                if (i == 2 && stop[j] == false)
                {
                    setBoolMatrix(matrix, dest_row[j], dest_col[j], true);
                }
            }
        }
        break;
    }
    case WARRIOR:
    {
        // 右下角，左下角，左上角，右上角
        int
            dest_row[4] = {src_row + 1, src_row + 1, src_row - 1, src_row - 1},
            dest_col[4] = {src_col + 1, src_col - 1, src_col - 1, src_col + 1};
        for (int i = 0; i < 4; i++)
        {
            if (isInsidePalace(board, dest_row[i], dest_col[i]))
            {
                setBoolMatrix(matrix, dest_row[i], dest_col[i], true);
            }
        }
        break;
    }
    case SOLDIER:
    {
        if (isInside(src_row, src_col) && isBeyond(board, src_row, src_col))
        {
            // 越过领地（楚河汉界），可以向右左走
            setBoolMatrix(matrix, src_row, src_col + 1, true);
            setBoolMatrix(matrix, src_row, src_col - 1, true);
        }
        // 无论何时都可以向前走（当前也没法向后走了
        if (board->user == PLAYER_1)
            setBoolMatrix(matrix, src_row - 1, src_col, true);
        if (board->user == PLAYER_2)
            setBoolMatrix(matrix, src_row + 1, src_col, true);
        break;
    }
    }
    return matrix;
}

char** pathFindingByPos(struct ChessBoard *board, int src_row, int src_col) {
    struct Chess* chess = board->block[src_row][src_col];
    return pathFindingByChess(board, chess);
}

void moveablePosition(struct ChessBoard* board, int src_row, int src_col) {
    reset_moveablePos(board);
    freeMatrix(board->moveablePos);
    board->moveablePos = pathFindingByPos(board, src_row, src_col);
}

// 是否符合棋子移动规则
bool isMoveable(struct ChessBoard* board, int src_row, int src_col, int dest_row, int dest_col) {
    return board->moveablePos[dest_row][dest_col] == true;
}

/**
 * @brief 判断游戏是否结束（是否将死）
 * 
 * @param board 棋盘
 * @return bool 返回true，如果已经将死；否则返回false
 */
bool isGameEnd(struct ChessBoard* board) {
    for (int i = 0; i < 2; i++)
    {
        if ((isChessStackEmpty(board->dead_chess[i]) == false) && (ChessStackTop(board->dead_chess[i])->type == GENERAL))
        {
            return true;
        }
    }
    return false;
    // if (
    // (isChessStackEmpty(board->dead_player1) == false && ChessStackTop(board->dead_player1)->type == GENERAL) || 
    // (isChessStackEmpty(board->dead_player2) == false && ChessStackTop(board->dead_player2)->type == GENERAL))
    //     return true;
    // else return false;
}

/**
 * @brief 行动完成，可移动路径图层和已选择棋子清空
 * 
 * @param board 棋盘
 */
void actionFinished(struct ChessBoard *board) {
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            board->moveablePos[i][j] = false;
        } 
    }
    board->chessChoose = NULL;
}

void aiLogic(struct ChessBoard *board, int *dest_row, int *dest_col) {
    ChessPtr chess;
    for (int i = 0; i <= board->objects->top; i++)
    {
        if (board->objects->stack[i]->owner != board->user) {
            chess = board->objects->stack[i];
            pathFindingByChess(board, chess);
            break;
        }
    }
    for (int i = 0; i < BOARD_ROW; i++)
    {
        for (int j = 0; j < BOARD_COL; j++)
        {
            if (!isNull(board, i, j) && (board->block[i][j]->owner != board->user)) // 非空且为敌人
            {
            }
        }
    }
    
}