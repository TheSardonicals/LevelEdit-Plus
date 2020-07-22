#pragma once
#include "headers.h"

#ifndef camera_h
#define camera_h

class Camera{
    public:
    Camera();
    ~Camera();

    void init(int width, int height, int speed, int cs = 3);
    void Show(SDL_Renderer*);

    private:
    int xpos;
    int ypos;
    int speed = speed;
    int cs = cs;

};

#endif /* Camera_h */