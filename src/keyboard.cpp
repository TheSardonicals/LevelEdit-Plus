#include "keyboard.h"

KeyboardManager::KeyboardManager(){
    SDL_memset(previous_keystate, 0, sizeof(bool)*SDL_SCANCODE_COUNT);
    SDL_memset(current_keystate, 0, sizeof(bool)*SDL_SCANCODE_COUNT);
}

void KeyboardManager::Process(){
    const bool* instance_keystate = SDL_GetKeyboardState(NULL);
    for (int key = 0; key < SDL_SCANCODE_COUNT; key++){
        if (SDL_GetKeyboardState(&key)){
            previous_keystate[key] = current_keystate[key];
        }
    }
    for (int key = 0; key < SDL_SCANCODE_COUNT; key++){
        if (SDL_GetKeyboardState(&key)){
            current_keystate[key] = instance_keystate[key];
        }
    }
}

bool KeyboardManager::KeyIsPressed(int scancode){
    return current_keystate[scancode];
}

bool KeyboardManager::KeyWasPressed(int scancode){
    return !previous_keystate[scancode] && current_keystate[scancode];
}