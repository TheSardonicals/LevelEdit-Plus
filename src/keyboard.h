#pragma once
#include "headers.h"

#ifndef keyboard_h
#define keyboard_h

class KeyboardManager {
    private:
        const bool* instance_keystate;
        bool previous_keystate[SDL_SCANCODE_COUNT];
        bool current_keystate [SDL_SCANCODE_COUNT];

    public:
        KeyboardManager();
        void Process();
        bool KeyIsPressed (int scancode);
        bool KeyWasPressed (int scancode);

};

#endif /* keyboard_h */