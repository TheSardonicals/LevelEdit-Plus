#pragma once
#include "headers.h"

#ifndef keyboard_h
#define keyboard_h

class KeyboardManager {
    private:
        bool previous_keystate[SDL_SCANCODE_COUNT];
        bool current_keystate [SDL_SCANCODE_COUNT];
        const bool* instance_keystate;

    public:
        KeyboardManager();
        void Process();
        bool KeyIsPressed (int scancode);
        bool KeyWasPressed (int scancode);

};

#endif /* keyboard_h */
