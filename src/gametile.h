#pragma once
#include "functions.h"

#include <array>
#include <SDL2/SDL.h>

using namespace std;

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