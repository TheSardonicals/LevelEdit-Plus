#include "camera.h"

Camera::Camera(float width, float height, float speed, float cs){
    xpos = 0;
    ypos = 0;
    this->speed = speed;
    this->cs = cs;
    Resize(width, height);
}

void Camera::Resize(float width, float height){
    // Inset by half of cs on every side, so the outline sits just inside the edges
    // of the window. In the docked layout that puts it under the panels at any
    // window size, which is where it sits at the default size.
    rect = {cs / 2, cs / 2, width - cs, height - cs};
}

void Camera::Show(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderRect(renderer, &rect);
}
