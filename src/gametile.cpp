#include "gametile.h"

GameTile::GameTile(TextureCache * _cache, char filepath, int xpos, int ypos, int w, int h){
    name = &filepath;
    cache = _cache;

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