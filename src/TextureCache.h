#pragma once
#include "headers.h"

#ifndef TextureCache_h
#define TextureCache_h

class TextureCache{
    public:
        TextureCache(SDL_Renderer *);
        ~TextureCache();

        void init(SDL_Renderer * );
        map<string, SDL_Texture*> LoadTexture(string);

        
    private:
    map<string, SDL_Texture *> _cache;
    SDL_Renderer * renderer;


};
#endif