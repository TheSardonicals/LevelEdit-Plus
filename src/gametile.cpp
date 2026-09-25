#include "gametile.h"
#include "functions.h"

GameTile::GameTile(TextureCache * cache, string filepath, int xpos, int ypos, int w = 32, int h = 32){

    // The name is the image's filename without its extension, taken from the *last*
    // path segment. Taking segment [1] only worked for "resources/Lava.bmp": a tile
    // imported from "exports/Level/assets/Lava.bmp" came out named "Level", so every
    // tile in an imported map shared one name and saving merged them into one entry.
    size_t slash = filepath.find_last_of("/\\");
    filename = (slash == string::npos) ? filepath : filepath.substr(slash + 1);
    size_t dot = filename.find_last_of('.');
    name = (dot == string::npos) ? filename : filename.substr(0, dot);

    // Whoever creates the tile overwrites this with the asset key it belongs to.
    // Falling back to the name keeps a tile made without one working exactly as
    // it did when every asset sat directly in resources/.
    key = name;
    this->cache = cache;
    this->filepath = filepath;

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

float GameTile::GroundLine() const{
    // The footprint runs from y - h/2 down to y + h/2, whatever the tile's height.
    return static_cast<float>(this->y) + (this->h / 2.0f);
}

void GameTile::Render(array<int, 2> camera_pos = {0, 0}, float alpha){
    // The top face is lifted by the tile's height. The footprint stays put, so a tile
    // that is raised still occupies the same ground it was placed on.
    this->rect.x = (this->x - (this->w/2)) + camera_pos[0];
    this->rect.y = (this->y - (this->h/2)) + camera_pos[1] - this->elevation;
    this->rect.w = this->w;
    this->rect.h = this->h;

    // Side face: fills the gap between the lifted top face and the ground. Drawn from
    // the tile's own texture with a darker colour mod, so every existing .bmp gets a
    // shaded side without anyone having to author new art for it.
    if (this->elevation > 0){
        SDL_FRect side = {this->rect.x, this->rect.y + this->rect.h,
                          static_cast<float>(this->w), static_cast<float>(this->elevation)};

        SDL_SetTextureColorMod(this->texture, 150, 150, 150);
        SDL_SetTextureAlphaMod(this->texture, (alpha * 255));
        SDL_RenderTexture(cache->renderer, this->texture, NULL, &side);
        // Shared texture out of the cache, so put the colour back for everyone else.
        SDL_SetTextureColorMod(this->texture, 255, 255, 255);
    }
    // nitpick (isaboll1): This might as well use the cache->SetTextureAlpha() function, that uses floats instead.
    // for consistency sake.
    SDL_SetTextureAlphaMod(this->texture, (alpha * 255));
    SDL_RenderTexture(cache->renderer, this->texture, NULL, &rect);

    // Outline the whole standing shape, top face plus side, so a raised tile reads as
    // one block rather than as a floating square.
    SDL_FRect outline = {this->rect.x, this->rect.y, this->rect.w,
                         this->rect.h + this->elevation};

    // Selection wins over hover, so a selected tile does not change colour just because
    // the mouse happens to be sitting on it.
    if (this->selected){
      SDL_SetRenderDrawColor(cache->renderer, 255, 145, 0, (alpha * 255));
      SDL_RenderRect(cache->renderer, &outline);
    }
    else if (this->highlight){
      SDL_SetRenderDrawColor(cache->renderer, 255, 255, 0, (alpha * 100));
      SDL_RenderRect(cache->renderer, &outline);
    }
}

void GameTile::SetPos(int xpos, int ypos){
    this->x = xpos;
    this->y = ypos;
}

array<int, 4> GameTile::GetInfo(){
    return {this->x, this->y, this->w , this->h};
}
