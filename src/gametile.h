#pragma once
#include "functions.h"
#include "texturecache.h"

#include <array>
#include <SDL2/SDL.h>

using namespace std;

#ifndef gametile_h
#define gametile_h

class GameTile{
    public:
        GameTile(TextureCache *, string, int, int, int, int);
        ~GameTile();

        string name;
        string filename;
        int x, y, w, h;
        SDL_Rect rect = {};
        TextureCache * cache;
        SDL_Texture * texture;

        void Render(array<int, 2>, float alpha = 1);
        void SetPos(int, int);

        array<int, 2> GetPos();
        array<int, 4> GetInfo();

};
#endif