#pragma once
#include "headers.h"

#ifndef texturecache_h
#define texturecache_h

class TextureCache{
    public:
        TextureCache(SDL_Renderer *);
        ~TextureCache();

        void init(SDL_Renderer * );
        map<string, SDL_Texture*> LoadTexture(string);

        map<string, SDL_Texture *> _cache;
        SDL_Renderer * renderer;

        
    private:
    


};
#endif