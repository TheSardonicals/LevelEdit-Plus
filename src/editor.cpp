#include "headers.h"

Editor::Editor(){
    //menu_items["Title"] = new TextObject("Map Editor", 400, 190, {"arcade", "font/arcade.ttf"}, {0, 0, 0}, {200, 100}, 12);
};

int Editor::Start(int argc, char** argv){
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow("LevelEdit Plus - by Sardonicals", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS);
    
    if (!window){
        ShowError("LevelEdit Plus Error!", "Couldn't create window: ", "Window creation failed!: ", true);
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
    if(!renderer){
        ShowError("LevelEdit Plus Error!", "Couldn't create renderer: ", "Renderer creation failed!: ", true);
        return 0;
    }

    event = SDL_Event();

    mouse = new Pointer();
    cache = new TextureCache(renderer);
    camera = new Camera();
    submenu = new Submenu();

    return 1;
}

void Editor::Loop(){
    while (running){
        Process();
        Render();
        SDL_Delay(5);
    }
}

void Editor::Process(){
    mouse->Process();

    while(SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            running = false;
            break;
        }
    }

    if (state == "MENU"){
        
    }

    if (state == "EDITING"){
        
    }
}

void Editor::Render(){

    
}