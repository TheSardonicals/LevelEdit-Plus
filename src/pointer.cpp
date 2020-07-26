#include "pointer.h"
#include "headers.h"

Pointer::Pointer(){
    xpos = 0;
    ypos = 0;
    clicking = false;
    r_clicking = false;
    clicked = false;
    has_clicked = false;
    pointer_rect = {0, 0, 10, 10};
}

void Pointer::Compute(SDL_Event* event){
    this->clicking = false;
    this->r_clicking = false;

    if (event->type == SDL_MOUSEMOTION){
        xpos = event->motion.x;
        ypos = event->motion.y;
    }
}

void Pointer::Process(){
    clicking = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(1);

    r_clicking = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(2);

    has_clicked = clicked && !clicking;

    clicked = clicking;

    pointer_rect.x = (xpos - int(pointer_rect.w / 2));
    pointer_rect.y = (ypos - int(pointer_rect.h / 2));
}

bool Pointer::Is_Clicking(SDL_Rect* rect){
    return Is_Touching(rect) && clicking;
}

bool Pointer::Is_R_Clicking(SDL_Rect* rect){
    return Is_Touching(rect) && r_clicking;
}

bool Pointer::Is_Touching(SDL_Rect* rect){
    return SDL_HasIntersection(&pointer_rect, rect);
}

bool Pointer::Has_Clicked(SDL_Rect* rect){
    return Is_Touching(rect) && has_clicked;
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