#include "TextureCache.h"

TextureCache::TextureCache(SDL_Renderer* target){
    renderer = target;
}

TextureCache::LoadTexture(string filepath){
    if (_cache.count(filepath) < 1){
        SDL_Surface* surface = SDL_LoadBMP(const char* filepath);
        _cache[filepath] = SDL_CreateTextureFromSurface(TextureCache->renderer, surface);
        SDL_FreeSurface(surface);
        SDL_SetTextureBlendMode(_cache[filepath], SDL_BLENDMODE_BLEND);

    }
}