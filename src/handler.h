#pragma once
#include "pointer.h"

using namespace std;

#ifndef handler_h
#define handler_h

class Handler{
    public:
        Handler(Pointer*);
        ~Handler();

        bool OptionClicked(string);
        void Render(int);

    private:
        Pointer * mouse;

};


#endif /* Handler_h */