#pragma once
#include "headers.h"

#ifndef gametile_h
#define gametile_h

class GameTile{
    public:
        GameTile(TextureCache*, string, int, int, int, int);
        ~GameTile();

        void Render(array<int, 2>, int);
        void SetPos(int, int);
        array<int, 2> GetPos();
        array<int, 4> GetInfo();
    
    private:
        TextureCache* cache;
        string name;
        int x, y, w, h;
        SDL_Rect* rect;


        
};
#endif