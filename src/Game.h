#pragma once
#include "headers.h"

#ifndef Game_h
#define Game_h

class Game{
    public:
        Game();
        ~Game();

        void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

        void handleEvents();
        void update();
        void render();
        void clean();

        bool running();

    private:

};
#endif /* Game_h */