#include "gametile.h"
#include "functions.h"

GameTile::GameTile(TextureCache * cache, string filepath, int xpos, int ypos, int w, int h){

    string filename = split(filepath, '/')[-1];
    name = split(filename, '.bmp')[0];
    this->cache = cache;

    texture = cache->LoadTexture(filepath);
    
    this->rect.x = xpos;
    this->rect.y = ypos;
    this->rect.w = w;
    this->rect.h = h;
}

void GameTile::Render(TextureCache * cache, array<int, 2> camera_pos = {0, 0}, int alpha = 255){
    this->rect.x = (this->x - (this->w/2)) + camera_pos[0];
    this->rect.y = (this->y - (this->h/2)) + camera_pos[1];
    this->rect.w = this->w;
    this->rect.h = this->h;
    // nitpick (isaboll1): This might as well use the cache->SetTextureAlpha() function, that uses floats instead.
    // for consistency sake.
    SDL_SetTextureAlphaMod(this->texture, alpha);
    SDL_RenderCopy(cache->renderer, this->texture, NULL, &rect);
}

void GameTile::SetPos(int xpos, int ypos){
    this->x = xpos;
    this->y = ypos;
}

array<int, 4> GameTile::GetInfo(){
    return {this->x, this->y, this->w , this->h};
}