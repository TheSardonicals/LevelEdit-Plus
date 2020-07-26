#include "headers.h"

GameTile::GameTile(TextureCache* cache, string filepath, int xpos, int ypos, int w, int h){
    string name = split(filepath, '.bmp')[0];

    this->_cache = cache;
    //texture = _cache.LoadTexture(filepath);
    
    this->rect.x = xpos;
    this->rect.y = ypos;
    this->rect.w = w;
    this->rect.h = h;
}

void GameTile::Render(array<int, 2> camera_pos = {0, 0}, int alpha = 255){
    this->rect.x = (this->x - (this->w/2)) + camera_pos[0];
    this->rect.y = (this->y - (this->h/2)) + camera_pos[1];
    this->rect.w = this->w;
    this->rect.h = this->h;
    SDL_SetTextureAlphaMod(this->texture, alpha);
    //SDL_RenderCopy(_cache.renderer, this->texture, NULL, this->rect);
}

void GameTile::SetPos(int xpos, int ypos){
    this->x = xpos;
    this->y = ypos;
}

array<int, 4> GameTile::GetInfo(){
    return {this->x, this->y, this->w , this->h};
}