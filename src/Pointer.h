#pragma once
#include "headers.h"

#ifndef Pointer_h
#define Pointer_h

class Pointer{
    public:
        Pointer();
        ~Pointer();

        void init();

        void compute(SDL_Event* event);
        void Set_Cursor(SDL_Cursor*);
        
        bool Is_Touching_Rect(SDL_Rect* rect);
        bool Is_Clicking_Rect(SDL_Rect* rect);
        bool Is_Touching(SDL_Rect* rect);
        bool Is_Clicking(TextObject*);
        bool Is_R_Clicking(TextObject*);
        

    private:
        int xpos = 0, ypos = 0;
        map<string, SDL_Cursor*> cursor;
        SDL_Rect* pointer;
        
        bool clicking = false;
        bool r_clicking = false;

};

#endif