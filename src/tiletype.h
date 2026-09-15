#pragma once

#include <array>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#ifndef tiletype_h
#define tiletype_h

// What a tile *means*, as opposed to what it looks like. There is one of these per
// tile type - per entry under "tiles" in an .mx - shared by every placement of it.
//
// The editor does not interpret any of this. It lets an author say what a tile is,
// stores it in the map, and leaves acting on it to whichever game loads the map.
// That is what lets one tileset carry meanings for several games at once: each game
// acts on the flags it knows and ignores the rest.
struct TileType {
    // Free-form meanings, lowercase, e.g. "solid", "explosive", "water". There is no
    // fixed list; see kKnownTileFlags for the ones the Inspector offers as checkboxes.
    vector<string> flags;

    // Whether anyone has actually said what this tile means - the map it came from
    // had a "flags" key, or an author has set them here. Until then `flags` holds
    // only a guess from the tile's name, which is shown but never saved.
    //
    // That distinction is the whole point. Games read a map with no "flags" key
    // the way they always have (BoxDead from tile names, DreamQuest from its own
    // block), and those disagree: writing BoxDead's guess into a DreamQuest map
    // would turn 11 of its tile types solid or explosive just because the map was
    // opened and saved here.
    bool declared = false;

    // Optional collision box smaller than the art, as [x, y, w, h] relative to the
    // top-left corner of the tile's quad - so a tree can block at its trunk and not
    // its canopy. When has_collision is false the whole quad is the box.
    bool has_collision = false;
    array<int, 4> collision = {0, 0, 32, 32};

    bool HasFlag(const string & flag) const;
    void SetFlag(const string & flag, bool on);
};

// Flags with a meaning a game already acts on, offered in the Inspector as
// checkboxes. Anything else is equally valid - it is just typed in by hand.
// Each entry is the flag and a one-line description for its tooltip.
extern const vector<pair<string, string>> kKnownTileFlags;

bool IsKnownTileFlag(const string & flag);

// Lowercase, trimmed, inner spaces as underscores, so "Blocks Shots " and
// "blocks_shots" are the same flag to every game that reads the map.
string NormaliseTileFlag(const string & flag);

// A guess at what a tile means from its name, using the keywords BoxDead inferred
// collision from before maps could say it outright. Shown in the Inspector as a
// starting point for an author; it is not written to the map until they accept or
// change it, because other games never guessed from names at all.
vector<string> SuggestFlagsFromName(const string & name);

// The type for `key`, created undeclared with a suggested guess if this is the
// first time the editor has seen it. Used everywhere a type is looked up, so the
// editor and its panels can never disagree about what an unseen type starts as.
TileType & EnsureTileType(map<string, TileType> & types, const string & key);

#endif
