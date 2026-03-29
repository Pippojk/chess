#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>

int bk1[4] = {105, 220, 76, 255};
int bk2[4] = {47,47, 47, 255};

std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

char board[8][8];

std::map<char, SDL_Texture*> textures;

int offsetX, offsetY, boardSize;

void loadFEN(const std::string& fen, char board[8][8]){
    int i = 0, j = 0;
    for(char c : fen){
        if(c == '/'){ i++; j = 0; continue; }
        if(std::isdigit(c)){
            j += c - '0';
            for(int k = 0; k< c-'0'; k++){
                board[i][k] = '.';
            }
        } else {
            board[i][j] = c;
            j++;
        }
    }
}

void drawBoard(SDL_Renderer* renderer, int w, int h){
    int square = std::min(w, h) / 8;
    boardSize = square * 8;

    offsetX = (w - boardSize) / 2;
    offsetY = (h - boardSize) / 2;

    int iMap = 0;

    SDL_Rect rectBoard = {offsetX, offsetY, boardSize, boardSize};

    SDL_RenderCopy(renderer, textures['c'], nullptr, &rectBoard);

    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            char current = board[i][j];

            // pezzo
            SDL_Texture* tex = textures[current];
            SDL_Rect rect = {offsetX + j*square,offsetY + i*square,square,square};

            if(tex){
                SDL_RenderCopy(renderer, tex, nullptr, &rect);
            }

            iMap++;
        }
    }
}


int main(){
    int w = 800;
    int h = 800;

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout<<"sdl video non funzionante";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("chess", 
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        w, h, 
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if(!window){
        std::cout<<"sdl window non funzionante";
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        std::cout<<"sdl renderer non funzionante";
        return 1;
    }

    // Caricamento SDL_image
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

    bool needed = true;
    bool running = true;
    int Sx, Sy;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }else if(event.type == SDL_WINDOWEVENT){
                if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    w = event.window.data1;
                    h = event.window.data2;

                    SDL_RenderSetViewport(renderer, NULL);
                }
            }else if(event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                SDL_GetMouseState(&x, &y);

                x -= offsetX; y -= offsetY;

                x /= (boardSize/8); y /= (boardSize/8);

                if(board[y][x] != '.'){
                    Sx = x;
                    Sy = y;
                }
            }else if(event.type == SDL_MOUSEBUTTONUP){
                int x, y;
                SDL_GetMouseState(&x, &y);

                x -= offsetX; y -= offsetY;

                x /= (boardSize/8); y /= (boardSize/8);

                board[y][x] = board[Sy][Sx];
                board[Sy][Sx] = '.';
            }
        }

        SDL_SetRenderDrawColor(renderer, 170, 170, 170, 255);
        SDL_RenderClear(renderer);
    
        drawBoard(renderer, w,h);           

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