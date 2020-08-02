#include "gametile.h"
#include "functions.h"

GameTile::GameTile(TextureCache * cache, string filepath, int xpos, int ypos, int w, int h){

    string filename = Split(filepath, '/')[1];
    name = Split(filename, '.')[0];
    this->cache = cache;

    texture = cache->LoadTexture(filepath);

    this->x = xpos;
    this->y = ypos;
    this->w = w;
    this->h = h;
    
    this->rect.x = xpos;
    this->rect.y = ypos;
    this->rect.w = w;
    this->rect.h = h;
}

GameTile::~GameTile(){}

void GameTile::Render(array<int, 2> camera_pos = {0, 0}, float alpha){
    this->rect.x = (this->x - (this->w/2)) + camera_pos[0];
    this->rect.y = (this->y - (this->h/2)) + camera_pos[1];
    this->rect.w = this->w;
    this->rect.h = this->h;
    // nitpick (isaboll1): This might as well use the cache->SetTextureAlpha() function, that uses floats instead.
    // for consistency sake.
    SDL_SetTextureAlphaMod(this->texture, (alpha * 255));
    SDL_RenderCopy(cache->renderer, this->texture, NULL, &rect);
}

void GameTile::SetPos(int xpos, int ypos){
    this->x = xpos;
    this->y = ypos;
}

array<int, 4> GameTile::GetInfo(){
    return {this->x, this->y, this->w , this->h};
}