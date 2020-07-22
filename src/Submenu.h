#include "headers.h"

#ifndef Submenu_h
#define Submenu_h

class Submenu{
    public:
        Submenu();
        ~Submenu();

        void init(TextureCache*, vector<string, tuple<int, int>>, tuple<int, int>, tuple<int, int>, tuple<int, int, int>,
        int, bool, tuple<int, int, int>);
    
    private:
        map<string, TextObject*> menu_item;
        map<string, GameTile*> icons;
        map<string, bool> selectable;

        SDL_Rect* area;

        tuple<int, int, int> scroll_color;

        int boundary_x, boundary_y;

        bool scrolling = true;
        bool activated = false;

        Pointer* mouse = NULL;
        
};

#endif