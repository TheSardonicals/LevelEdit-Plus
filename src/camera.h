#pragma once
#include "headers.h"

#ifndef camera_h
#define camera_h

class Camera{
    public:
        Camera(int, int, int, int);
        ~Camera();

        void Show(SDL_Renderer*);

        SDL_Rect rect = {};

        int xpos;
        int ypos;
        int speed;
        int cs;
        
        

};

#endif /* Camera_h */