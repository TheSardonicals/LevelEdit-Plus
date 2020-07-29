#include "headers.h"

#ifndef submenu_h
#define submenu_h

class Submenu{
    public:
        Submenu();
        ~Submenu();

        void init(vector<string, array<int, 2>>, array<int, 2>, array<int, 2>, array<int, 3>,
        int, bool, array<int, 3>);
    
    private:
        //map<string, GameTile*> icons;
        map<string, bool> selectable;

        SDL_Rect* area;

        array<int, 3> scroll_color;

        int boundary_x, boundary_y;

        bool scrolling = true;
        bool activated = false;

        
};

#endif