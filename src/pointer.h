#pragma once
#include <SDL2/SDL.h>
#include <map>
#include <string>

using namespace std;

#ifndef pointer_h
#define pointer_h

class Pointer{
    public:
        Pointer();
        ~Pointer();

        void init();

        void Compute(SDL_Event* event);

        void Set_Cursor(SDL_Cursor*);
        
        bool IsTouchingRect(SDL_Rect* rect);
        bool IsClickingRect(SDL_Rect* rect);
        bool IsTouching(SDL_Rect* rect);
        bool IsClicking(SDL_Rect*);
        bool IsRClicking(SDL_Rect*);
        bool HasClicked(SDL_Rect*);

        void Render(SDL_Renderer*);

        void Process();
        
        int xpos = 0, ypos = 0;
        map<string, SDL_Cursor*> cursor;

        bool has_clicked;
        

    private:
        SDL_Rect pointer_rect;
        bool clicking;
        bool r_clicking;
        bool clicked;

};

#endif