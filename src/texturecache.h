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

        SDL_Texture * LoadTexture(char);
        SDL_Renderer * renderer;
        
    private:
        map<char, SDL_Texture *> textures = {};
    


};
#endif