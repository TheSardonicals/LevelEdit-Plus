#include "texturecache.h"

TextureCache::TextureCache(SDL_Renderer* target){
    renderer = target;
}

SDL_Texture * TextureCache::LoadTexture(char filepath){
    if (textures.find(filepath) == textures.end()){
        SDL_Surface * surface = SDL_LoadBMP(&filepath);
        if (!surface){
            ShowError("Level Editor error!", (filepath + "not found!, can't load tile"), "file not loaded", false);
        }
        textures[filepath] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_SetTextureBlendMode(textures[filepath], SDL_BLENDMODE_BLEND);
    }
    return textures[filepath];
}
TextureCache::~TextureCache(){
    for (auto const &instance : textures){
        SDL_DestroyTexture(instance.second);
    }
}