#pragma once
#include "headers.h"

#ifndef Camera_h
#define Camera_h

class Camera{
    public:
    Camera();
    ~Camera();

    void init(int width, int height, int speed, int cs = 40);
    void Show(SDL_Renderer*);

    private:
    int xpos;
    int ypos;
    int speed = speed;
    int cs = cs;

};

#endif /* Camera_h */