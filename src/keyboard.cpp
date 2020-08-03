#include "keyboard.h"

KeyboardManager::KeyboardManager(){
    SDL_memset(previous_keystate, 0, sizeof(Uint8)*SDL_NUM_SCANCODES);
    SDL_memset(current_keystate, 0, sizeof(Uint8)*SDL_NUM_SCANCODES);
    instance_keystate = SDL_GetKeyboardState(NULL);
}

void KeyboardManager::Process(){

    for (int key = 0; key < SDL_NUM_SCANCODES; key++){
        previous_keystate[key] = current_keystate[key];
    }
    for (int key = 0; key < SDL_NUM_SCANCODES; key++){
        current_keystate[key] = instance_keystate[key];
    }
}

bool KeyboardManager::KeyIsPressed(int scancode){
    return current_keystate[scancode];
}

bool KeyboardManager::KeyWasPressed(int scancode){
    return !previous_keystate[scancode] && current_keystate[scancode];
}