#pragma once
#include "headers.h"

#ifndef camera_h
#define camera_h

class Camera{
    public:
        Camera(float, float, float, float);
        ~Camera();

        void Show(SDL_Renderer*);

        SDL_FRect rect = {};

        float xpos;
        float ypos;
        float speed;
        float cs;
        
        

};

#endif /* Camera_h */