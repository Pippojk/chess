#include <iostream>
#include <limits>
#include "engine.h"
#include "game.h"


Engine::Engine(){
    kingScheme = {
        {-30,-40,-40,-50,-50,-40,-40,-30,},
        {-30,-40,-40,-50,-50,-40,-40,-30,},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}
    };

    knightScheme = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50},
    };

    bishopsScheme = {
        {-20,-10,-10,-10,-10,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5, 10, 10,  5,  0,-10},
        {-10,  5,  5, 10, 10,  5,  5,-10},
        {-10,  0, 10, 10, 10, 10,  0,-10},
        {-10, 10, 10, 10, 10, 10, 10,-10},
        {-10,  5,  0,  0,  0,  0,  5,-10},
        {-20,-10,-10,-10,-10,-10,-10,-20},
    };

    rooksScheme = {
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 5, 10, 10, 10, 10, 10, 10,  5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        { 0,  0,  0,  5,  5,  0,  0,  0}
    };

    queenScheme = {
        {-20,-10,-10, -5, -5,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5,  5,  5,  5,  0,-10},
        {-5,  0,  5,  5,  5,  5,  0, -5},
        {0,  0,  5,  5,  5,  5,  0, -5},
        {-10,  5,  5,  5,  5,  5,  0,-10},
        {-10,  0,  5,  0,  0,  0,  0,-10},
        {-20,-10,-10, -5, -5,-10,-10,-20}
    };

    pawnScheme = {
        {0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {5,  5, 10, 25, 25, 10,  5,  5},
        {0,  0,  0, 20, 20,  0,  0,  0},
        { 5, -5,-10,  0,  0,-10, -5,  5},
        { 5, 10, 10,-20,-20, 10, 10,  5},
        { 0,  0,  0,  0,  0,  0,  0,  0}
    };
}

void Engine::getboard(std::vector<std::vector<char>> chessboard){
    simulation.changeBoard(chessboard);
}

float Engine::getEvaluation(){
    return evaluation;
}


void Engine::clearMoves(){
    moves.clear();
}

int findMin(const std::vector<float>& lista){
    if(lista.empty()) return -1;
    int min = 0;
    for(int i = 1; i < (int)lista.size(); i++){  // fix: start at 1
        if(lista[i] < lista[min]) min = i;
    }
    return min;
}

int findMax(const std::vector<float>& lista){
    if(lista.empty()) return -1;
    int max = 0;
    for(int i = 1; i < (int)lista.size(); i++){  // fix: start at 1
        if(lista[i] > lista[max]) max = i;
    }
    return max;
}

void Engine::setEvaluation(){
    evaluation = 0.0f;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            switch(simulation.board[i][j]){
                case 'P': evaluation += (100.0f + pawnScheme[i][j]);  break;
                case 'p': evaluation -= 100.0f + pawnScheme[7-i][j];  break;
                case 'B': evaluation += 330.00f + bishopsScheme[i][j];  break;
                case 'b': evaluation -= 330.00f + bishopsScheme[7-i][j];  break;
                case 'N': evaluation += 320.0f + knightScheme[i][j];  break;
                case 'n': evaluation -= 320.0f + knightScheme[7-i][j];  break;
                case 'R': evaluation += 500.0f + rooksScheme[i][j];  break;
                case 'r': evaluation -= 500.0f + rooksScheme[7-i][j];  break;
                case 'Q': evaluation += 900.0f + queenScheme[i][j];  break;
                case 'q': evaluation -= 900.0f + queenScheme[7-i][j];  break;
                case 'K': evaluation += 2000.0f + kingScheme[i][j]; break;
                case 'k': evaluation -= 2000.0f + kingScheme[7-i][j]; break;
                default:  break;
            }
        }
    }
}

void Engine::findMoves(bool isWhiteTurn){
    clearMoves();
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(simulation.board[i][j] != '.' &&
               !isWhiteTurn && islower(simulation.board[i][j]))
            {
                fromToMove m;
                m.inizio = {j, i};
                simulation.checkLegalMoves(j, i);
                
                // Filtra mosse che lasciano il re in scacco
                std::vector<Move> legal;
                for(auto& mv : simulation.getMoves()){
                    if(!simulation.wouldBeInCheck(j, i, mv.x, mv.y, false)){
                        legal.push_back(mv);
                    }
                }
                m.possibili = legal;
                
                if(!m.possibili.empty())
                    moves.push_back(m);
            }
        }
    }
}

float Engine::tryMove(Move start, Move end){  // fix: return float
    std::vector<std::vector<char>> saved(8, std::vector<char>(8));
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            saved[i][j] = simulation.board[i][j];


    if(simulation.applyMove(start.x, start.y, end.x, end.y, false) == -1){
        return std::numeric_limits<float>::max();;
    }

    setEvaluation(); 
    float result = getEvaluation();

    getboard(saved);
    return result;
}

// engine.cpp

float Engine::minimax(int depth, bool isMaximizing){
    // Caso base: profondità 0, ritorna valutazione statica
    if(depth == 0){
        setEvaluation();
        return getEvaluation();
    }

    // Genera tutte le mosse per il giocatore corrente
    std::vector<fromToMove> currentMoves;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            char piece = simulation.board[i][j];
            if(piece == '.') continue;
            bool isPieceWhite = isupper(piece);
            if(isPieceWhite != isMaximizing) continue;

            fromToMove m;
            m.inizio = {j, i};
            simulation.checkLegalMoves(j, i);

            for(auto& mv : simulation.getMoves()){
                if(!simulation.wouldBeInCheck(j, i, mv.x, mv.y, isMaximizing)){
                    m.possibili.push_back(mv);
                }
            }
            if(!m.possibili.empty()) currentMoves.push_back(m);
        }
    }

    if(currentMoves.empty()){
        // Nessuna mossa = scacco matto o stallo
        if(simulation.isInCheck(isMaximizing)){
            return isMaximizing ? -1000.0f : 1000.0f;  // scacco matto
        }
        return 0.0f;  // stallo
    }

    if(isMaximizing){
        float best = -1000.0f;
        for(auto& m : currentMoves){
            for(auto& mv : m.possibili){
                // Salva e applica la mossa
                std::vector<std::vector<char>> saved = simulation.getBoard();
                simulation.applyMove(m.inizio.x, m.inizio.y, mv.x, mv.y, isMaximizing);

                // Ricorsione
                float score = minimax(depth - 1, false);
                best = std::max(best, score);

                // Ripristina
                simulation.changeBoard(saved);
            }
        }
        return best;
    } else {
        float best = 1000.0f;
        for(auto& m : currentMoves){
            for(auto& mv : m.possibili){
                std::vector<std::vector<char>> saved = simulation.getBoard();
                simulation.applyMove(m.inizio.x, m.inizio.y, mv.x, mv.y, isMaximizing);

                float score = minimax(depth - 1, true);
                best = std::min(best, score);

                simulation.changeBoard(saved);
            }
        }
        return best;
    }
}

fromToMove Engine::shearch(int deep){
    if(moves.empty()) return fromToMove{};

    float bestScore = 1000.0f;  // nero minimizza
    int bestMoveIdx = 0;

    for(int idx = 0; idx < (int)moves.size(); idx++){
        auto& m = moves[idx];
        if(m.possibili.empty()) continue;

        float ris = tryMove(m.inizio, m.possibili[0]);
        m.best = m.possibili[0];

        for(int i = 0; i < (int)m.possibili.size(); i++){
            // Salva stato
            std::vector<std::vector<char>> saved = simulation.getBoard();
            simulation.applyMove(m.inizio.x, m.inizio.y, m.possibili[i].x, m.possibili[i].y, false);

            // Minimax dalla posizione risultante
            float score = minimax(deep - 1, true);  // tocca al bianco rispondere

            // Ripristina
            simulation.changeBoard(saved);

            if(score < ris){
                ris = score;
                m.best = m.possibili[i];
            }
        }

        if(ris < bestScore){
            bestScore = ris;
            bestMoveIdx = idx;
        }
    }

    return moves[bestMoveIdx];
}