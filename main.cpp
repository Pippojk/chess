#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <vector>
#include <cctype>


int bk1[4] = {105, 220, 76, 255};
int bk2[4] = {47,47, 47, 255};

std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

char board[8][8];

struct Move{
    int x, y;
};

Move BenPassant[2] = {{-1, -1}, {-1, -1}};//il primo e sempre a sinistra il secondo sempre a destra
Move WenPassant[2] = {{-1, -1}, {-1, -1}};//il primo e sempre a sinistra il secondo sempre a destra

std::vector<Move> moves;

//per sapere se le torri o i re si sono mossi li rapresento 
//in queste due liste, la lettura e da sinistra a destra, dall'alto 
//al basso, quindi il primo 0 dei re e il re nero e cosi via
int kingMoved[2] = {0, 0};
int rookMoved[4] = {0, 0, 0, 0};

bool isWhiteTurn = true;


std::map<char, SDL_Texture*> textures;

int offsetX, offsetY, boardSize;
int Sx, Sy = -1;
void loadFEN(const std::string& fen, char board[8][8]){
    int i = 0, j = 0;
    for(char c : fen){
        if(c == '/'){ i++; j = 0; continue; }
        if(std::isdigit(c)){
            for(int k = 0; k< c-'0'; k++){
                board[i][j] = '.';
                j ++;
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

            if(Sx == j && Sy == i){
                SDL_SetRenderDrawColor(renderer, 221, 255, 107, 255);
                SDL_RenderFillRect(renderer, &rect);
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                rect.x = mouseX-square/2; rect.y = mouseY-square/2;
            }

            if(tex){
                SDL_RenderCopy(renderer, tex, nullptr, &rect);
            }

            iMap++;
        }
    }
}

void pawnMoves(int x, int y, bool isWhite){
    if(isWhite){
        //controllo se posso fare l'enPassant
        if(WenPassant[0].x == x && WenPassant[0].y == y){
            moves.push_back({x+1, y-1});
        }

        if(WenPassant[1].x == x && WenPassant[1].y == y){
            moves.push_back({x-1, y-1});
        }

        //mossa di uno avanti
        if(y > 0 && board[y-1][x] == '.'){
            moves.push_back({x, y-1});
        }
        //mossa di due avanti
        if(y == 6 && board[y-1][x] == '.' && board[y-2][x] == '.'){
            moves.push_back({x, y-2});

            //controllo per pedoni che possono fare enPassant
            if(islower(board[y-2][x-1])){
                BenPassant[0] = {x-1, y-2};
            }
            if(islower(board[y-2][x+1])){
                BenPassant[1] = {x+1, y-2};
            }
        }
        //mangiare a destra
        if(y > 0 && islower(board[y-1][x+1])){
            moves.push_back({x+1, y-1});
        }
        //mangiare a sinistra
        if(y > 0 && islower(board[y-1][x-1])){
            moves.push_back({x-1, y-1});
        }
    }else{
        //controllo se posso fare l'enPassant
        if(BenPassant[0].x == x && BenPassant[0].y == y){
            moves.push_back({x+1, y+1});
        }

        if(BenPassant[1].x == x && BenPassant[1].y == y){
            moves.push_back({x-1, y+1});
        }

        //mossa di uno avanti
        if(y > 0 && board[y+1][x] == '.'){
            moves.push_back({x, y+1});
        }
        //mossa di due avanti
        if(y == 1 && board[y+1][x] == '.' && board[y+2][x] == '.'){
            moves.push_back({x, y+2});

            if(isupper(board[y+2][x-1])){
                WenPassant[0] = {x-1, y+2};
            }
            if(isupper(board[y+2][x+1])){
                WenPassant[1] = {x+1, y+2};
            }
        }
        //mangiare a destra
        if(y > 0 && isupper(board[y+1][x+1])){
            moves.push_back({x+1, y+1});
        }
        //mangiare a sinistra
        if(y > 0 && isupper(board[y+1][x-1])){
            moves.push_back({x-1, y+1});
        }
    }
}

void rookMoves(int x, int y, bool isWhite){
    int i = y+1;
    while(board[i][x] == '.'){
        moves.push_back({x, i});
        i ++;
    }
    if(isWhite){
        if(islower(board[i][x])) moves.push_back({x, i});
    }else{
       if(isupper(board[i][x])) moves.push_back({x, i});
    }

    i = y-1;
    while(board[i][x] == '.'){
        moves.push_back({x, i});
        i --;
    }
    if(isWhite){
        if(islower(board[i][x])) moves.push_back({x, i});
    }else{
       if(isupper(board[i][x])) moves.push_back({x, i});
    }

    i = x-1;
    while(board[y][i] == '.'){
        moves.push_back({i, y});
        i --;
    }
    if(isWhite){
        if(islower(board[y][i])) moves.push_back({i, y});
    }else{
       if(isupper(board[y][i])) moves.push_back({i, y});
    }
        
    i = x+1;
    while(board[y][i] == '.'){
        moves.push_back({i, y});
        i ++;
    }
    if(isWhite){
        if(islower(board[y][i])) moves.push_back({i, y});
    }else{
       if(isupper(board[y][i])) moves.push_back({i, y});
    }
}

void kingMoves(int x, int y, bool isWhite){
    int kingMoves[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

    for(auto m : kingMoves){
        if(board[y+m[0]][x+m[1]] == '.'){
            moves.push_back({x+m[1], y+m[0]});
        }else if(isWhite){
            if(islower(board[y+m[0]][x+m[1]])){
                moves.push_back({x+m[1], y+m[0]});
            }
        }else{
            if(isupper(board[y+m[0]][x+m[1]])){
                moves.push_back({x+m[1], y+m[0]});
            }
        }
    }

    if(isWhite){
        if(kingMoved[1] == 0 && rookMoved[3] == 0){
            if(board[7][5] == '.' && board[7][6] == '.') moves.push_back({6, 7});
        }
        if(kingMoved[1] == 0 && rookMoved[2] == 0){
            if(board[7][3] == '.' && board[7][2] == '.' && board[7][1] == '.') moves.push_back({2, 7});    
        }
    }else{
        if(kingMoved[0] == 0 && rookMoved[1] == 0){
            if(board[0][5] == '.' && board[0][6] == '.') moves.push_back({6, 0});
        }
        if(kingMoved[0] == 0 && rookMoved[0] == 0){
            if(board[0][3] == '.' && board[0][2] == '.' && board[0][1] == '.') moves.push_back({2, 0});    
        }
    }
}

void bishopMoves(int x, int y, bool isWhite){
    int i = x+1;
    int j = y+1;
    while(board[j][i] == '.'){
        moves.push_back({i, j});
        i ++;
        j ++;
    }
    if(isWhite){ 
        if(islower(board[j][i])) moves.push_back({i, j});
    }else{
        if(isupper(board[j][i])) moves.push_back({i, j});
    }

    i = x+1;
    j = y-1;
    while(board[j][i] == '.'){
        moves.push_back({i, j});
        i ++;
        j --;
    }
    if(isWhite){ 
        if(islower(board[j][i])) moves.push_back({i, j});
    }else{
        if(isupper(board[j][i])) moves.push_back({i, j});
    }

    i = x-1;
    j = y+1;
    while(board[j][i] == '.'){
        moves.push_back({i, j});
        i --;
        j ++;
    }
    if(isWhite){ 
        if(islower(board[j][i])) moves.push_back({i, j});
    }else{
        if(isupper(board[j][i])) moves.push_back({i, j});
    }

    i = x-1;
    j = y-1;
    while(board[j][i] == '.'){
        moves.push_back({i, j});
        i --;
        j --;
    }
    if(isWhite){ 
        if(islower(board[j][i])) moves.push_back({i, j});
    }else{
        if(isupper(board[j][i])) moves.push_back({i, j});
    }
}

void queenMoves(int x, int y, bool isWhite){
    rookMoves(x, y, isWhite);
    bishopMoves(x,  y,  isWhite);
}

void knightMoves(int x, int y, bool isWhite){
    int knightMoves[8][2] = {{1, 2}, {-1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, 1}, {-2, -1}};
    for(auto m : knightMoves){
        if(board[y+m[1]][x+m[0]] == '.'){
            moves.push_back({x+m[0], y+m[1]});
        }else if(isWhite){
            if(islower(board[y+m[1]][x+m[0]])){
                moves.push_back({x+m[0], y+m[1]});
            }
        }else if(!isWhite){
            if(isupper(board[y+m[1]][x+m[0]])){
                moves.push_back({x+m[0], y+m[1]});
            }
        }
    }
}

void checkLegalMoves(int startX, int startY){
    char piece = board[startY][startX];

    if(piece == 'P' || piece == 'p') pawnMoves(startX, startY, isupper(piece));
    else if(piece == 'R' || piece == 'r') rookMoves(startX, startY, isupper(piece));
    else if(piece == 'K' || piece == 'k') kingMoves(startX, startY, isupper(piece));
    else if(piece == 'B' || piece == 'b') bishopMoves(startX, startY, isupper(piece));
    else if(piece == 'Q' || piece == 'q') queenMoves(startX, startY, isupper(piece));
    else if(piece == 'N' || piece == 'n') knightMoves(startX, startY, isupper(piece));
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
                    if((isWhiteTurn && isupper(board[y][x])) || (!isWhiteTurn && islower(board[y][x]))){
                        Sx = x;
                        Sy = y;
                    }
                }

                checkLegalMoves(Sx, Sy);
            }else if(event.type == SDL_MOUSEBUTTONUP){
                int x, y;
                SDL_GetMouseState(&x, &y);

                x -= offsetX; y -= offsetY;

                x /= (boardSize/8); y /= (boardSize/8);

                Move SelectedMove = {x, y};

                //controllo se mosso il re 

                for(Move m : moves){
                    if((x != Sx || y != Sy) && SelectedMove.x == m.x && SelectedMove.y == m.y){
                        //controllo se si sono mossi re o torri per arrocco
                        if(board[Sy][Sx] == 'k'){
                            kingMoved[0] = 1;
                        }else if(board[Sy][Sx] == 'K'){
                            kingMoved[1] = 1;
                        }else if(board[Sy][Sx] == 'r' && Sy == 0 && Sx == 0){
                            kingMoved[0] = 1;
                        }else if(board[Sy][Sx] == 'r' && Sy == 0 && Sx == 7){
                            kingMoved[1] = 1;
                        }else if(board[Sy][Sx] == 'R' && Sy == 7 && Sx == 0){
                            kingMoved[2] = 1;
                        }else if(board[Sy][Sx] == 'R' && Sy == 7 && Sx == 7){
                            kingMoved[3] = 1;
                        }

                        //controllo se si sta arroccando per spostare torre
                        if(board[Sy][Sx] == 'k'){
                            if(Sx - x > 1 || Sx - x < 1){
                                if(x == 2){
                                    board[0][3] = 'r';
                                    board[0][0] = '.';
                                }else{
                                    board[0][5] = 'r';
                                    board[0][7] = '.';
                                }
                            }
                        }else if(board[Sy][Sx] == 'K'){
                            if(Sx - x > 1 || Sx - x < 1){
                                if(x == 2){
                                    board[7][3] = 'R';
                                    board[7][0] = '.';
                                }else{
                                    board[7][5] = 'R';
                                    board[7][7] = '.';
                                }
                            }
                        }

                        //controllo enPassant e promozione
                        if(board[Sy][Sx] == 'p'){
                            //controllo promozione
                            if(y == 7){
                                char selected;
                                std::cout<<"scegliere in cosa promuovere(b, q, r, n): ";
                                std::cin>>selected;
                                if(selected != 'b' && selected != 'q' && selected != 'r' && selected != 'n'){
                                    return 0;
                                }
                                board[Sy][Sx] = '.';
                                board[y][x] = selected;
                                isWhiteTurn = !isWhiteTurn;
                                break;
                            }
                            //controllo enPassant
                            if(x!=Sx && y!=Sy && board[y][x] == '.'){
                                board[y-1][x] = '.';
                            }
                        }else if(board[Sy][Sx] == 'P'){
                            //controllo promozione
                            if(y == 0){
                                char selected;
                                std::cout<<"scegliere in cosa promuovere(B, Q, R, N): ";
                                std::cin>>selected;
                                if(selected != 'B' && selected != 'Q' && selected != 'R' && selected != 'N'){
                                    return 0;
                                }
                                board[Sy][Sx] = '.';
                                board[y][x] = selected;
                                isWhiteTurn = !isWhiteTurn;
                                break;
                            }

                            if(x!=Sx && y!=Sy && board[y][x] == '.'){
                                board[y+1][x] = '.';
                            }
                        }

                        board[y][x] = board[Sy][Sx];
                        board[Sy][Sx] = '.';
                        if(isWhiteTurn){ WenPassant[0] = {-1, -1}; WenPassant[1] = {-1, -1};}
                        else {BenPassant[0] = {-1, -1}; BenPassant[1] = {-1, -1};}
                        isWhiteTurn = !isWhiteTurn;
                    }
                }
                    
                Sy = -1; Sx = -1;
                moves.clear();
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