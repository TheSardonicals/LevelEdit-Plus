#include "pointer.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include <iostream>

Pointer::Pointer(){
    this->xpos = 0;
    this->ypos = 0;
    clicking = false;
    r_clicking = false;
    clicked = false;
    has_clicked = false;
    pointer_rect = {0, 0, 10, 10};
}

void Pointer::Compute(SDL_Event* event){
    // Instead of checking for SDL_MOUSEMOTION event, We Will use a change in the mouse's state from its relative state from last called.
    
    if (SDL_GetRelativeMouseState(&this->xpos, &this->ypos) != SDL_GetMouseState(&this->xpos, &this->ypos)){
        this->xpos = event->motion.x;
        this->ypos = event->motion.y;
    }

}

void Pointer::Process(){
    
    clicking = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(1);

    r_clicking = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(2);
    
    has_clicked = clicked && !clicking;
    
    clicked = clicking;

}

bool Pointer::IsClicking(SDL_Rect* rect){
    return IsTouching(rect) && clicking;
}

bool Pointer::IsRClicking(SDL_Rect* rect){
    return IsTouching(rect) && r_clicking;
}

bool Pointer::IsTouching(SDL_Rect* rect){
    return SDL_HasIntersection(&pointer_rect, rect);
}

bool Pointer::HasClicked(SDL_Rect* rect){
    return IsTouching(rect) && has_clicked;
}

void Pointer::Render(SDL_Renderer* renderer){
    if (clicking){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pointer_rect);
    }
    else if (has_clicked){
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &pointer_rect);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &pointer_rect);
    }
}