#pragma once
#include "headers.h"

#ifndef textobject_h
#define textobject_h

class TextObject{
    public:
        TextObject();
        ~TextObject();

        void init(SDL_Renderer* renderer, string text, int width, int height, vector<int> color, vector<int> location, int font_size);
        void Render(int, int, int);
        
};
#endif