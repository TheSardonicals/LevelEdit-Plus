#include "headers.h"

#ifndef DynamicTextObject_h
#define DynamicTextObject_h

class DynamicTextObject{

    public:
    DynamicTextObject(SDL_Renderer*, TTF_Font*, int, vector<int>);
    ~DynamicTextObject();

    void RenderText(string, string, tuple<int, int, int>, int);
};

#endif /* DynamicTextObject_h */