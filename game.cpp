#include <iostream>
#include <map>
#include <vector>
#include "game.h"

Game::Game(char chessboard[8][8]){
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            this->board[i][j] = chessboard[i][j];
        }
    }
}

Move Game::findking(bool isWhite){
    char target = isWhite ? 'K' : 'k';
    for(int i = 0; i<8; i++)
        for(int j = 0; j<8; j++)
            if(board[i][j] == target)
                return {j, i};
    return {-1, -1};
}

bool Game::isInCheck(bool isWhite){
    Move k = findking(isWhite);
    int x = k.x;
    int y = k.y;

    if(x == -1 || y == -1) return false; // re non trovato

    // Direzioni torre/regina
    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for(auto d : dirs){
        int i = x + d[0];
        int j = y + d[1];
        while(i >= 0 && i < 8 && j >= 0 && j < 8){
            char p = board[j][i];
            if(p != '.'){
                if(isWhite && islower(p) && (p == 'r' || p == 'q')) return true;
                if(!isWhite && isupper(p) && (p == 'R' || p == 'Q')) return true;
                break;
            }
            i += d[0];
            j += d[1];
        }
    }

    // Direzioni alfiere/regina
    int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for(auto d : diag){
        int i = x + d[0];
        int j = y + d[1];
        while(i >= 0 && i < 8 && j >= 0 && j < 8){
            char p = board[j][i];
            if(p != '.'){
                if(isWhite && islower(p) && (p == 'b' || p == 'q')) return true;
                if(!isWhite && isupper(p) && (p == 'B' || p == 'Q')) return true;
                break;
            }
            i += d[0];
            j += d[1];
        }
    }

    // Cavalli
    int knight[8][2] = {{1,2},{2,1},{-1,2},{-2,1},{1,-2},{2,-1},{-1,-2},{-2,-1}};
    for(auto m : knight){
        int i = x + m[0];
        int j = y + m[1];
        if(i>=0 && i<8 && j>=0 && j<8){
            char p = board[j][i];
            if(isWhite && p == 'n') return true;
            if(!isWhite && p == 'N') return true;
        }
    }

    // Pedoni
    if(isWhite){
        if(y > 0){
            if(x > 0 && board[y-1][x-1] == 'p') return true;
            if(x < 7 && board[y-1][x+1] == 'p') return true;
        }
    } else {
        if(y < 7){
            if(x > 0 && board[y+1][x-1] == 'P') return true;
            if(x < 7 && board[y+1][x+1] == 'P') return true;
        }
    }

    // Re avversario
    for(int dx = -1; dx <= 1; dx++){
        for(int dy = -1; dy <= 1; dy++){
            if(dx == 0 && dy == 0) continue;
            int i = x + dx;
            int j = y + dy;
            if(i>=0 && i<8 && j>=0 && j<8){
                char p = board[j][i];
                if(isWhite && p == 'k') return true;
                if(!isWhite && p == 'K') return true;
            }
        }
    }

    return false;
}

bool Game::wouldBeInCheck(int sx, int sy, int dx, int dy, bool isWhite){
    char temp[8][8];
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            temp[i][j] = board[i][j];

    board[dy][dx] = board[sy][sx];
    board[sy][sx] = '.';

    bool check = isInCheck(isWhite);

    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            board[i][j] = temp[i][j];

    return check;
}

void Game::drawBoard(SDL_Renderer* renderer, int w, int h, std::map<char, SDL_Texture*> textures, int offsetX, int offsetY){
    int square = std::min(w, h) / 8;
    int boardSize = square * 8;

    offsetX = (w - boardSize) / 2;
    offsetY = (h - boardSize) / 2;

    SDL_Rect rectBoard = {offsetX, offsetY, boardSize, boardSize};
    SDL_RenderCopy(renderer, textures['c'], nullptr, &rectBoard);

    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            char current = board[i][j];
            SDL_Texture* tex = textures[current];
            SDL_Rect rect = {offsetX + j*square, offsetY + i*square, square, square};

            if(Sx == j && Sy == i){
                SDL_SetRenderDrawColor(renderer, 221, 255, 107, 255);
                SDL_RenderFillRect(renderer, &rect);
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                rect.x = mouseX - square/2;
                rect.y = mouseY - square/2;
            }

            if(tex){
                SDL_RenderCopy(renderer, tex, nullptr, &rect);
            }
        }
    }
}

void Game::pawnMoves(int x, int y, bool isWhite){
    if(isWhite){
        // en passant
        if(WenPassant[0].x == x && WenPassant[0].y == y) moves.push_back({x+1, y-1});
        if(WenPassant[1].x == x && WenPassant[1].y == y) moves.push_back({x-1, y-1});

        // avanti di uno
        if(y > 0 && board[y-1][x] == '.'){
            moves.push_back({x, y-1});
        }
        // avanti di due
        if(y == 6 && board[y-1][x] == '.' && board[y-2][x] == '.'){
            moves.push_back({x, y-2});
            if(x > 0 && islower(board[y-2][x-1])) BenPassant[0] = {x-1, y-2};
            if(x < 7 && islower(board[y-2][x+1])) BenPassant[1] = {x+1, y-2};
        }
        // cattura destra
        if(y > 0 && x < 7 && islower(board[y-1][x+1])) moves.push_back({x+1, y-1});
        // cattura sinistra
        if(y > 0 && x > 0 && islower(board[y-1][x-1])) moves.push_back({x-1, y-1});
    } else {
        // en passant
        if(BenPassant[0].x == x && BenPassant[0].y == y) moves.push_back({x+1, y+1});
        if(BenPassant[1].x == x && BenPassant[1].y == y) moves.push_back({x-1, y+1});

        // avanti di uno
        if(y < 7 && board[y+1][x] == '.'){
            moves.push_back({x, y+1});
        }
        // avanti di due
        if(y == 1 && board[y+1][x] == '.' && board[y+2][x] == '.'){
            moves.push_back({x, y+2});
            if(x > 0 && isupper(board[y+2][x-1])) WenPassant[0] = {x-1, y+2};
            if(x < 7 && isupper(board[y+2][x+1])) WenPassant[1] = {x+1, y+2};
        }
        // cattura destra
        if(y < 7 && x < 7 && isupper(board[y+1][x+1])) moves.push_back({x+1, y+1});
        // cattura sinistra
        if(y < 7 && x > 0 && isupper(board[y+1][x-1])) moves.push_back({x-1, y+1});
    }
}

void Game::rookMoves(int x, int y, bool isWhite){
    // Giù
    for(int i = y+1; i < 8; i++){
        if(board[i][x] == '.') moves.push_back({x, i});
        else {
            if(isWhite && islower(board[i][x])) moves.push_back({x, i});
            if(!isWhite && isupper(board[i][x])) moves.push_back({x, i});
            break;
        }
    }
    // Su
    for(int i = y-1; i >= 0; i--){
        if(board[i][x] == '.') moves.push_back({x, i});
        else {
            if(isWhite && islower(board[i][x])) moves.push_back({x, i});
            if(!isWhite && isupper(board[i][x])) moves.push_back({x, i});
            break;
        }
    }
    // Destra
    for(int i = x+1; i < 8; i++){
        if(board[y][i] == '.') moves.push_back({i, y});
        else {
            if(isWhite && islower(board[y][i])) moves.push_back({i, y});
            if(!isWhite && isupper(board[y][i])) moves.push_back({i, y});
            break;
        }
    }
    // Sinistra
    for(int i = x-1; i >= 0; i--){
        if(board[y][i] == '.') moves.push_back({i, y});
        else {
            if(isWhite && islower(board[y][i])) moves.push_back({i, y});
            if(!isWhite && isupper(board[y][i])) moves.push_back({i, y});
            break;
        }
    }
}

void Game::kingMoves(int x, int y, bool isWhite){
    int km[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};
    for(auto m : km){
        int nx = x + m[1];
        int ny = y + m[0];
        if(nx < 0 || nx >= 8 || ny < 0 || ny >= 8) continue;
        if(board[ny][nx] == '.') moves.push_back({nx, ny});
        else if(isWhite && islower(board[ny][nx])) moves.push_back({nx, ny});
        else if(!isWhite && isupper(board[ny][nx])) moves.push_back({nx, ny});
    }

    // Arrocco bianco
    if(isWhite){
        if(kingMoved[1] == 0 && rookMoved[3] == 0)
            if(board[7][5] == '.' && board[7][6] == '.') moves.push_back({6, 7});
        if(kingMoved[1] == 0 && rookMoved[2] == 0)
            if(board[7][3] == '.' && board[7][2] == '.' && board[7][1] == '.') moves.push_back({2, 7});
    } else {
        if(kingMoved[0] == 0 && rookMoved[1] == 0)
            if(board[0][5] == '.' && board[0][6] == '.') moves.push_back({6, 0});
        if(kingMoved[0] == 0 && rookMoved[0] == 0)
            if(board[0][3] == '.' && board[0][2] == '.' && board[0][1] == '.') moves.push_back({2, 0});
    }
}

void Game::bishopMoves(int x, int y, bool isWhite){
    int diags[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for(auto d : diags){
        int i = x + d[0];
        int j = y + d[1];
        while(i >= 0 && i < 8 && j >= 0 && j < 8){
            if(board[j][i] == '.') moves.push_back({i, j});
            else {
                if(isWhite && islower(board[j][i])) moves.push_back({i, j});
                if(!isWhite && isupper(board[j][i])) moves.push_back({i, j});
                break;
            }
            i += d[0];
            j += d[1];
        }
    }
}

void Game::queenMoves(int x, int y, bool isWhite){
    rookMoves(x, y, isWhite);
    bishopMoves(x, y, isWhite);
}

void Game::knightMoves(int x, int y, bool isWhite){
    int km[8][2] = {{1,2},{-1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,1},{-2,-1}};
    for(auto m : km){
        int nx = x + m[0];
        int ny = y + m[1];
        if(nx < 0 || nx >= 8 || ny < 0 || ny >= 8) continue;
        if(board[ny][nx] == '.') moves.push_back({nx, ny});
        else if(isWhite && islower(board[ny][nx])) moves.push_back({nx, ny});
        else if(!isWhite && isupper(board[ny][nx])) moves.push_back({nx, ny});
    }
}

void Game::checkLegalMoves(int startX, int startY){
    moves.clear();
    char piece = board[startY][startX];
    if(piece == 'P' || piece == 'p') pawnMoves(startX, startY, isupper(piece));
    else if(piece == 'R' || piece == 'r') rookMoves(startX, startY, isupper(piece));
    else if(piece == 'K' || piece == 'k') kingMoves(startX, startY, isupper(piece));
    else if(piece == 'B' || piece == 'b') bishopMoves(startX, startY, isupper(piece));
    else if(piece == 'Q' || piece == 'q') queenMoves(startX, startY, isupper(piece));
    else if(piece == 'N' || piece == 'n') knightMoves(startX, startY, isupper(piece));
}



void Game::checkMove(int x, int y){
    if(x < 0 || x >= 8 || y < 0 || y >= 8) return;
    if(board[y][x] != '.'){
        if((isWhiteTurn && isupper(board[y][x])) || (!isWhiteTurn && islower(board[y][x]))){
            Sx = x;
            Sy = y;
        }
    }
}

void Game::changeTurn(){
    isWhiteTurn = !isWhiteTurn;
}

int Game::makeMove(Move SelectedMove, int x, int y){
    if(Sx == -1 || Sy == -1) return 0;
    if(x < 0 || x >= 8 || y < 0 || y >= 8) return 0;

    for(Move m : moves){
        if((x != Sx || y != Sy) && SelectedMove.x == m.x && SelectedMove.y == m.y){
            if(wouldBeInCheck(Sx, Sy, x, y, isWhiteTurn)){
                continue;
            }

            // Aggiorna stato arrocco
            if(board[Sy][Sx] == 'k') kingMoved[0] = 1;
            else if(board[Sy][Sx] == 'K') kingMoved[1] = 1;
            else if(board[Sy][Sx] == 'r' && Sy == 0 && Sx == 0) rookMoved[0] = 1;
            else if(board[Sy][Sx] == 'r' && Sy == 0 && Sx == 7) rookMoved[1] = 1;
            else if(board[Sy][Sx] == 'R' && Sy == 7 && Sx == 0) rookMoved[2] = 1;
            else if(board[Sy][Sx] == 'R' && Sy == 7 && Sx == 7) rookMoved[3] = 1;

            // Arrocco: muovi la torre
            if(board[Sy][Sx] == 'k' && std::abs(Sx - x) == 2){
                if(x == 2){ board[0][3] = 'r'; board[0][0] = '.'; }
                else       { board[0][5] = 'r'; board[0][7] = '.'; }
            } else if(board[Sy][Sx] == 'K' && std::abs(Sx - x) == 2){
                if(x == 2){ board[7][3] = 'R'; board[7][0] = '.'; }
                else       { board[7][5] = 'R'; board[7][7] = '.'; }
            }

            // Promozione / en passant pedone nero
            if(board[Sy][Sx] == 'p'){
                if(y == 7){
                    char selected;
                    std::cout << "Promuovere in (b, q, r, n): ";
                    std::cin >> selected;
                    if(selected != 'b' && selected != 'q' && selected != 'r' && selected != 'n') return 0;
                    board[Sy][Sx] = '.';
                    board[y][x] = selected;
                    isWhiteTurn = !isWhiteTurn;
                    Sx = -1; Sy = -1;
                    return 1;
                }
                if(x != Sx && board[y][x] == '.') board[y-1][x] = '.'; // en passant
            } else if(board[Sy][Sx] == 'P'){
                if(y == 0){
                    char selected;
                    std::cout << "Promuovere in (B, Q, R, N): ";
                    std::cin >> selected;
                    if(selected != 'B' && selected != 'Q' && selected != 'R' && selected != 'N') return 0;
                    board[Sy][Sx] = '.';
                    board[y][x] = selected;
                    isWhiteTurn = !isWhiteTurn;
                    Sx = -1; Sy = -1;
                    return 1;
                }
                if(x != Sx && board[y][x] == '.') board[y+1][x] = '.'; // en passant
            }

            board[y][x] = board[Sy][Sx];
            board[Sy][Sx] = '.';
            if(isWhiteTurn){ WenPassant[0] = {-1,-1}; WenPassant[1] = {-1,-1}; }
            else            { BenPassant[0] = {-1,-1}; BenPassant[1] = {-1,-1}; }
            isWhiteTurn = !isWhiteTurn;
            break;
        }
    }

    Sx = -1; Sy = -1;
    return 1;
}

void Game::clearMoves(){
    moves.clear();
}

std::vector<std::vector<char>> Game::getBoard(){
    std::vector<std::vector<char>> chessboard(8, std::vector<char>(8));
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            chessboard[i][j] = board[i][j];
        }
    }

    return chessboard;

}

std::vector<Move> Game::getMoves(){
    return moves;
}

void Game::changeBoard(std::vector<std::vector<char>> chessboard){
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            board[i][j] = chessboard[i][j];
        }
    }
}

bool Game::getWhiteTurn(){
    return isWhiteTurn;
}

int Game::applyMove(int sx, int sy, int dx, int dy, bool IsWhiteTurn){
    if(!wouldBeInCheck(sx, sy, dx, dy, IsWhiteTurn)){
        board[dy][dx] = board[sy][sx];
        board[sy][sx] = '.';
        return 0;
    }else{
        return -1;
    }
    
}