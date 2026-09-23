#pragma once
#include "functions.h"
#include "texturecache.h"

#include <array>
#include <SDL3/SDL.h>

using namespace std;

#ifndef gametile_h
#define gametile_h

class GameTile{
    public:
        GameTile(TextureCache *, string, int, int, int, int);
        ~GameTile();

        string name;
        string filename;
        string filepath;

        // What this tile is filed under: its asset key ("tiles/grass"), or the
        // key it was imported under. `name` is only the image's filename, which
        // is not enough once assets live in folders - two folders can each hold
        // a "grass". Set by whoever creates the tile; defaults to the name.
        string key;
        int x, y, w = 32, h = 32;

        // How far the tile stands up off the ground, in pixels. 0 is a flat floor
        // tile. Anything taller draws its top face lifted by this much with a shaded
        // side face filling the gap down to the footprint, which is what gives the
        // top-down view its 2.5D look. The footprint itself never moves, so collision
        // and placement stay where the level author put them.
        int elevation = 0;

        // The top face: what gets drawn and what the mouse picks against. The
        // footprint sits `elevation` pixels below it.
        SDL_FRect rect = {};
        TextureCache * cache;
        SDL_Texture * texture;

        void Render(array<int, 2>, float alpha = 1);
        void SetPos(int, int);

        // Bottom edge of the ground footprint, in world pixels. Tiles are drawn in
        // order of this so a tall tile correctly covers whatever stands behind it.
        float GroundLine() const;

        array<int, 2> GetPos();
        array<int, 4> GetInfo();

        // highlight is the hover outline, selected is the stronger one drawn around every
        // tile in the current selection.
        bool highlight = false;
        bool selected = false;
};
#endif
