#pragma once
#include "headers.h"

#ifndef TextObject_h
#define TextObject_h

class TextObject{
    public:
        TextObject();
        ~TextObject();

        void init(SDL_Renderer* renderer, string text, int width, int height, vector<int> color, vector<int> location, int font_size);
        void Render();
        
};
#endif