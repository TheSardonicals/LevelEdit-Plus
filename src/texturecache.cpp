#include "texturecache.h"

TextureCache::TextureCache(SDL_Renderer* target){
    renderer = target;
}

TextureCache::~TextureCache(){
    for (auto texture: cache){
        SDL_DestroyTexture(texture.second);
    }
}

SDL_Texture* TextureCache::LoadTexture(string filepath){
    if (cache.count(filepath) < 1){
        SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
        cache[filepath] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_SetTextureBlendMode(cache[filepath], SDL_BLENDMODE_BLEND);
    }
    return cache[filepath];
}

void TextureCache::SetTextureAlpha(SDL_Texture * texture, float alpha){
    SDL_SetTextureAlphaMod(texture, (alpha * 255));
}