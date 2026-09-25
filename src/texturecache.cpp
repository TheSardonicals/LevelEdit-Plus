#include "texturecache.h"

// stb_image is header-only and was already sitting in src/, so PNG (and JPG, TGA,
// GIF...) support costs nothing to link or ship: the editor still needs only
// SDL3 beside it. The other vendored header, SDL_stbimage.h, is the SDL2 version
// and does not build against SDL3.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace {

// Loads any image the editor understands into a texture, or NULL.
//
// A .bmp still goes through SDL, which reads the palette and RLE forms of the
// format that stb_image does not, and which is what every existing tile is.
// Everything else goes through stb_image.
SDL_Texture * CreateTextureFromFile(SDL_Renderer * renderer, const string & filepath){
    if (FileExtension(filepath) == ".bmp"){
        SDL_Surface * surface = SDL_LoadBMP(filepath.c_str());
        if (!surface){
            return NULL;
        }
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        return texture;
    }

    // Four channels always, so a PNG keeps its transparency and one pixel format
    // covers everything regardless of what the file happened to store.
    int width = 0, height = 0, channels_in_file = 0;
    stbi_uc * pixels = stbi_load(filepath.c_str(), &width, &height, &channels_in_file, 4);
    if (!pixels){
        return NULL;
    }

    // The surface borrows these pixels rather than copying them, so the texture
    // has to be made before they are freed.
    SDL_Surface * surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32,
                                                  pixels, width * 4);
    SDL_Texture * texture = surface ? SDL_CreateTextureFromSurface(renderer, surface) : NULL;

    SDL_DestroySurface(surface);
    stbi_image_free(pixels);
    return texture;
}

}  // namespace

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
        SDL_Texture * texture = CreateTextureFromFile(renderer, filepath);

        if (texture){
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        }
        else {
            // Cached as NULL either way, so a broken file is not retried every
            // frame, but say which one it was: importing a folder of art from
            // another project is exactly when this happens.
            cout << "couldn't load image " << filepath << endl;
        }

        cache[filepath] = texture;
    }
    return cache[filepath];
}

void TextureCache::SetTextureAlpha(SDL_Texture * texture, float alpha){
    SDL_SetTextureAlphaMod(texture, (alpha * 255));
}
