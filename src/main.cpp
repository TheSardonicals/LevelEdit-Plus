#include "headers.h"

int Start(int argc, char* args[]){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = NULL;
    SDL_Event event;
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);

    if (SDL_INIT(SDL_INIT_VIDEO) < 0){
        printf("SDL_VIDEO not initialized");
    }else{
        while (running){
            
        }
    }
    return 0;
    
}