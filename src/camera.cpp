#include "camera.h"

Camera::Camera(float width, float height, float speed, float cs){
    xpos = 0;
    ypos = 0;
    this->speed = speed;
    this->cs = cs;
    rect = {cs / 2, cs / 2, width - cs, height - cs};
}

void Camera::Show(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderRect(renderer, &rect);
}
