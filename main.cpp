#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int arc, char* args[]){
    SDL_Window* win = NULL;

    SDL_Surface* screenSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO)  < 0){
        printf( "SDL could not initialize! SDL_Error %s\n", SDL_GetError());
    }else
    {
        win = SDL_CreateWindow("LevelEdit Plus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
        if (win == NULL){
            printf("Window could not be created! SDL_Error %s\n", SDL_GetError());
        }else
        {
            screenSurface = SDL_GetWindowSurface(win);

            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            SDL_UpdateWindowSurface(win);

            SDL_Delay(2000);
        }
        
    }

    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
    
}