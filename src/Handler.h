#include "headers.h"

#ifndef Handler_h
#define Handler_h

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