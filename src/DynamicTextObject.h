#include "headers.h"

#ifndef DynamicTextObject_h
#define DynamicTextObject_h

class DynamicTextObject{

    public:
    DynamicTextObject(SDL_Renderer*, TTF_Font*, int, array<int, 3>);
    ~DynamicTextObject();

    void RenderText(string, string, array<int, 3>, int);
};

#endif /* DynamicTextObject_h */