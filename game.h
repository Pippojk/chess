#pragma once  // aggiungi questa riga all'inizio
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>


struct Move{
    int x, y;
};

class Game{
public:
    Game() = default;
    Game(char chessboard[8][8]);

    bool isInCheck(bool isWhite);
    bool wouldBeInCheck(int sx, int sy, int dx, int dy, bool isWhite);
    void drawBoard(SDL_Renderer* renderer, int w, int h, std::map<char, SDL_Texture*> textures,  int offsetX, int offsetY);
    void checkLegalMoves(int startX, int startY);
    Move findking(bool isWhite);
    void checkMove(int x, int y);
    void changeTurn();
    bool getWhiteTurn();
    int makeMove(Move selectedMove, int x, int y);
    void clearMoves();
    std::vector<std::vector<char>> getBoard();
    std::vector<Move> getMoves();
    void changeBoard(std::vector<std::vector<char>> chessboard);
    int applyMove(int sx, int sy, int dx, int dy, bool IsWhiteTurn);
    char board[8][8];

private:
    Move BenPassant[2] = {{-1, -1}, {-1, -1}};//il primo e sempre a sinistra il secondo sempre a destra
    Move WenPassant[2] = {{-1, -1}, {-1, -1}};
    
    std::vector<Move> moves;
    int kingMoved[2] = {0, 0};
    int rookMoved[4] = {0, 0, 0, 0};
    int Sx = -1; int Sy = -1;
    bool isWhiteTurn = false;

    void pawnMoves(int x, int y, bool isWhite);
    void rookMoves(int x, int y, bool isWhite);
    void kingMoves(int x, int y, bool isWhite);
    void bishopMoves(int x, int y, bool isWhite);
    void queenMoves(int x, int y, bool isWhite);
    void knightMoves(int x, int y, bool isWhite);
};