#include <SDL2/SDL.h>

#ifndef TextureCache_h
#define TextureCache_h

class TextureCache{
    public:
        TextureCache();
        ~TextureCache();

        void init(SDL_Renderer* renderer);

        
    private:
        //cache holder here

};
#endif