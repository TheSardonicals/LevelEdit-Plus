#pragma once
#include <SDL3/SDL.h>
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
        
        bool IsTouchingRect(SDL_FRect*);
        bool IsClickingRect(SDL_FRect*);
        bool IsTouching(SDL_FRect*);
        bool IsClicking(SDL_FRect*);
        bool IsRClicking(SDL_FRect*);
        bool HasClicked(SDL_FRect*);

        void Render(SDL_Renderer*);

        void Process();
        
        float xpos = 0, ypos = 0;
        map<string, SDL_Cursor*> cursor;

        bool has_clicked;
        bool has_rclicked;
        

    private:
        SDL_FRect pointer_rect;
        bool clicking;
        bool r_clicking;
        bool r_clicked;
        bool clicked;

};

#endif
