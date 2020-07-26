#pragma once
#include "headers.h"

#ifndef textobject_h
#define textobject_h

class TextObject{
    public:
        TextObject(SDL_Renderer*, string, int, int, array<string, 2>, array<int, 3>, array<int, 2>, int);
        ~TextObject();
        void Render(int, int, int);
        
};
#endif