#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <vector>
#include <cctype>
#include "game.h"
#include "engine.h"

std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

char board[8][8];

std::map<char, SDL_Texture*> textures;

void loadFEN(const std::string& fen, char board[8][8]){
    int i = 0, j = 0;
    for(char c : fen){
        if(c == '/'){ i++; j = 0; continue; }
        if(std::isdigit(c)){
            for(int k = 0; k < c-'0'; k++){
                board[i][j] = '.';
                j++;
            }
        } else {
            board[i][j] = c;
            j++;
        }
    }
}

// Calcola offsetX, offsetY, boardSize dati w e h correnti
void calcLayout(int w, int h, int& offsetX, int& offsetY, int& boardSize){
    int square = std::min(w, h) / 8;
    boardSize = square * 8;
    offsetX = (w - boardSize) / 2;
    offsetY = (h - boardSize) / 2;
}

int main(){
    int w = 800;
    int h = 800;

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "sdl video non funzionante\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("chess",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          w, h,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if(!window){
        std::cout << "sdl window non funzionante\n";
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        std::cout << "sdl renderer non funzionante\n";
        return 1;
    }

    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cout << "SDL_image non funzionante: " << IMG_GetError() << "\n";
        return 1;
    }

    loadFEN(fen, board);

    textures['p'] = IMG_LoadTexture(renderer, "img/pieces/p.png");
    textures['P'] = IMG_LoadTexture(renderer, "img/pieces/P.png");
    textures['n'] = IMG_LoadTexture(renderer, "img/pieces/n.png");
    textures['N'] = IMG_LoadTexture(renderer, "img/pieces/N.png");
    textures['r'] = IMG_LoadTexture(renderer, "img/pieces/r.png");
    textures['R'] = IMG_LoadTexture(renderer, "img/pieces/R.png");
    textures['b'] = IMG_LoadTexture(renderer, "img/pieces/b.png");
    textures['B'] = IMG_LoadTexture(renderer, "img/pieces/B.png");
    textures['k'] = IMG_LoadTexture(renderer, "img/pieces/k.png");
    textures['K'] = IMG_LoadTexture(renderer, "img/pieces/K.png");
    textures['q'] = IMG_LoadTexture(renderer, "img/pieces/q.png");
    textures['Q'] = IMG_LoadTexture(renderer, "img/pieces/Q.png");
    textures['c'] = IMG_LoadTexture(renderer, "img/chessboard.png");

    Game game{board};
    game.changeTurn();
    Engine motore;
    bool engineMoved = false;
    // Prendi dimensione reale della finestra dopo MAXIMIZED
    SDL_GetWindowSize(window, &w, &h);

    int offsetX, offsetY, boardSize;
    calcLayout(w, h, offsetX, offsetY, boardSize);

    bool running = true;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            } else if(event.type == SDL_WINDOWEVENT){
                if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                    w = event.window.data1;
                    h = event.window.data2;
                    calcLayout(w, h, offsetX, offsetY, boardSize);
                    SDL_RenderSetViewport(renderer, NULL);
                }
            } else if(event.type == SDL_MOUSEBUTTONDOWN && game.getWhiteTurn()){
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                int cellSize = boardSize / 8;
                int bx = (mx - offsetX) / cellSize;
                int by = (my - offsetY) / cellSize;

                // Ignora click fuori dalla scacchiera
                if(bx < 0 || bx >= 8 || by < 0 || by >= 8) continue;

                game.checkMove(bx, by);
                game.checkLegalMoves(bx, by);

            } else if(event.type == SDL_MOUSEBUTTONUP && game.getWhiteTurn()){
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                int cellSize = boardSize / 8;
                int bx = (mx - offsetX) / cellSize;
                int by = (my - offsetY) / cellSize;

                if(bx < 0 || bx >= 8 || by < 0 || by >= 8){
                    game.clearMoves();
                    continue;
                }

                Move selectedMove = {bx, by};
                game.makeMove(selectedMove, bx, by);
                game.clearMoves();
            }
        }
SDL_SetRenderDrawColor(renderer, 170, 170, 170, 255);
        SDL_RenderClear(renderer);
        game.drawBoard(renderer, w, h, textures, offsetX, offsetY);

       if(!game.getWhiteTurn() && !engineMoved){
    motore.getboard(game.getBoard());
    motore.findMoves(false);

    fromToMove m = motore.shearch(4);

    if(!m.possibili.empty()){
        game.applyMove(m.inizio.x, m.inizio.y, m.best.x, m.best.y, game.getWhiteTurn());
        motore.clearMoves();
        game.changeTurn();
        engineMoved = true;
    }

} else if(game.getWhiteTurn()){
    engineMoved = false;
}

        

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    for(auto& t : textures){
        SDL_DestroyTexture(t.second);
    }
    IMG_Quit();
    SDL_Quit();
    return 0;
}