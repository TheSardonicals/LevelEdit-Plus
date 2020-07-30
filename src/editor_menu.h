#pragma once
#include "headers.h"
#include "gametile.h"

#ifndef editor_menu_h
#define editor_menu_h

class EditorMenu{
    public:
        bool hide_lower_options = false;
        EditorMenu(int *, int *, ImVec4 *, Pointer *, map<char, string> *);
        ~EditorMenu();
        void Process();
    private:
        bool show_item_menu = false;
        float alpha = 1.0f;
        bool hide_stats = false;
        bool align_menu_to_screen = true;
        int * window_width;
        int * window_height;
        ImVec4 * clear_color;
        Pointer * mouse;
        map<string, GameTile *> * tile_list;
        map<char, string> * tile_paths;
};
#endif