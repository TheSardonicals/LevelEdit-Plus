#pragma once
#include "headers.h"

#ifndef keyboard_h
#define keyboard_h

class KeyboardManager {
    private:
        const Uint8 * instance_keystate;
        Uint8 previous_keystate[SDL_SCANCODE_COUNT];
        Uint8 current_keystate [SDL_SCANCODE_COUNT];

    public:
        KeyboardManager();
        void Process();
        bool KeyIsPressed (int scancode);
        bool KeyWasPressed (int scancode);

};

#endif /* keyboard_h */