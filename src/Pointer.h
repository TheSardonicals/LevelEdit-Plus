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
        
        bool Is_Touching_Rect(SDL_Rect* rect);
        bool Is_Clicking_Rect(SDL_Rect* rect);
        bool Is_Touching(SDL_Rect* rect);

};

#endif