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

// The flags a tile type starts with when its map declares none: the same name
// keywords games were inferring collision from before maps could say it outright.
// A map saved before flags existed therefore keeps behaving exactly as it did -
// the difference is that the guess is now visible in the Inspector and can be
// corrected, instead of being made silently at runtime.
vector<string> SuggestFlagsFromName(const string & name);

// The type for `key`, created with suggested flags if this is the first time the
// editor has seen it. Used everywhere a type is looked up, so the editor and its
// panels can never disagree about what an unseen type starts as.
TileType & EnsureTileType(map<string, TileType> & types, const string & key);

#endif
