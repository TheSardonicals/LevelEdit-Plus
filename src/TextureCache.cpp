#include "TextureCache.h"

TextureCache::TextureCache(SDL_Renderer* target){
    renderer = target;
}

map<string, SDL_Texture*> TextureCache::LoadTexture(string filepath){
    if (_cache.count(filepath) < 1){
        SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
        _cache[filepath] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_SetTextureBlendMode(_cache[filepath], SDL_BLENDMODE_BLEND);

    }
}