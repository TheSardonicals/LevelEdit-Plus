#pragma once
#include "headers.h"

#ifndef camera_h
#define camera_h

class Camera{
    public:
        Camera(float, float, float, float);
        ~Camera();

        void Show(SDL_Renderer*);

        // Fits the border to a new output size. Called whenever the window changes
        // size, so the border keeps hugging the window's edges instead of staying
        // the size the window was when the editor started.
        void Resize(float width, float height);

        SDL_FRect rect = {};

        float xpos;
        float ypos;
        float speed;
        float cs;
        
        

};

#endif /* Camera_h */