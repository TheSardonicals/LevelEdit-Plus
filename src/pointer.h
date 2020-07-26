#pragma once
#include "headers.h"

#ifndef pointer_h
#define pointer_h

class Pointer{
    public:
        Pointer();
        ~Pointer();

        void init();

        void Compute(SDL_Event* event);

        void Set_Cursor(SDL_Cursor*);
        
        bool Is_Touching_Rect(SDL_Rect* rect);
        bool Is_Clicking_Rect(SDL_Rect* rect);
        bool Is_Touching(SDL_Rect* rect);
        bool Is_Clicking(SDL_Rect*);
        bool Is_R_Clicking(SDL_Rect*);
        bool Has_Clicked(SDL_Rect*);

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