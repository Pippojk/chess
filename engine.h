#pragma once  // aggiungi questa riga all'inizio
#include <iostream>
#include <vector>
#include "game.h"

struct fromToMove{
    Move inizio;
    std::vector<Move> possibili;
    Move best;
    //float value = 0.00f;
};

class Engine{
public:
    Engine();
    void getboard(std::vector<std::vector<char>> chessboard);
    float getEvaluation();
    void setEvaluation();
    void findMoves(bool isWhiteTurn);
    fromToMove shearch(int deep);
    void clearMoves();

private:
    float minimax(int depth, bool isMaximizing);  // aggiungi questo
    Game simulation;
    float evaluation = 0.00f;
    std::vector<fromToMove> moves;
    std::vector<std::vector<int>> pawnScheme;
    std::vector<std::vector<int>> bishopsScheme;
    std::vector<std::vector<int>> knightScheme;
    std::vector<std::vector<int>> rooksScheme;
    std::vector<std::vector<int>> queenScheme;
    std::vector<std::vector<int>> kingScheme;


    float tryMove(Move start, Move end);
};