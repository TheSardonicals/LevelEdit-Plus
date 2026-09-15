#include "tiletype.h"

#include <algorithm>
#include <cctype>

const vector<pair<string, string>> kKnownTileFlags = {
    {"solid",     "Blocks movement"},
    {"explosive", "Destructible prop that blows up (BoxDead spawns a barrel instead of drawing a tile)"},
};

bool IsKnownTileFlag(const string & flag){
    for (auto & known : kKnownTileFlags){
        if (known.first == flag) return true;
    }
    return false;
}

bool TileType::HasFlag(const string & flag) const{
    return find(flags.begin(), flags.end(), flag) != flags.end();
}

void TileType::SetFlag(const string & flag, bool on){
    auto found = find(flags.begin(), flags.end(), flag);
    if (on && found == flags.end()){
        flags.push_back(flag);
    }
    else if (!on && found != flags.end()){
        flags.erase(found);
    }
}

string NormaliseTileFlag(const string & flag){
    size_t start = flag.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = flag.find_last_not_of(" \t");

    string out;
    for (size_t i = start; i <= end; ++i){
        char c = flag[i];
        out.push_back(c == ' ' ? '_' : static_cast<char>(tolower(static_cast<unsigned char>(c))));
    }
    return out;
}

vector<string> SuggestFlagsFromName(const string & name){
    string lower;
    for (char c : name){
        lower.push_back(static_cast<char>(tolower(static_cast<unsigned char>(c))));
    }

    // Explosive is checked first, so "Explosive Barrel" is a barrel and not a wall.
    // These lists match what BoxDead matched on, so an old map is read the same way.
    static const char * explosive[] = {"barrel", "drum", "explosive", "tnt"};
    for (const char * keyword : explosive){
        if (lower.find(keyword) != string::npos) return {"explosive"};
    }

    static const char * solid[] = {"wall", "block", "rock", "stone", "barrier",
                                   "fence", "crate", "pillar", "obstacle"};
    for (const char * keyword : solid){
        if (lower.find(keyword) != string::npos) return {"solid"};
    }

    return {};
}

TileType & EnsureTileType(map<string, TileType> & types, const string & key){
    auto found = types.find(key);
    if (found != types.end()){
        return found->second;
    }
    TileType & created = types[key];
    created.flags = SuggestFlagsFromName(key);
    return created;
}
