#pragma once
#include "headers.h"

#ifndef GameTile_h
#define GameTile_h

class GameTile{
    public:
        GameTile(TextureCache*, string, int, int, int, int);
        ~GameTile();

        void Render(tuple<int, int>, int);
        void SetPos(int, int);
        vector<int> GetPos();
        vector<int> GetInfo();
    
    private:
        TextureCache* cache;
        string name;
        int x, y, w, h;
        SDL_Rect* rect;


        
};
#endif