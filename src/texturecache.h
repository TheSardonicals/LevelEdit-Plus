#pragma once
#include <SDL2/SDL.h>
#include <map>

#include "functions.h"

using namespace std;

#ifndef texturecache_h
#define texturecache_h

class TextureCache{
    public:
        TextureCache(SDL_Renderer *);
        ~TextureCache();

        SDL_Texture* LoadTexture(string);

        // alpha values are between 0 - 1.
        void SetTextureAlpha(SDL_Texture * texture, float alpha = 1);
        SDL_Renderer * renderer;

    private:
        map<string, SDL_Texture *> cache = {};

};
#endif