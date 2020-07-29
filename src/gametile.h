#pragma once
#include "headers.h"

#ifndef gametile_h
#define gametile_h

class GameTile{
    public:
        GameTile(string, int, int, int, int);
        ~GameTile();

        string name;
        int x, y, w, h;
        SDL_Rect rect = {};
        SDL_Texture* texture = NULL;



        void Render(array<int, 2>, int);
        void SetPos(int, int);

        array<int, 2> GetPos();
        array<int, 4> GetInfo();

};
#endif