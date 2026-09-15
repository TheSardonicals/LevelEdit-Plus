#pragma once
#include "json.hpp"
#include "gametile.h"
#include "tiletype.h"
#include <algorithm>
#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <system_error>
#include <vector>

using namespace std;
using nlohmann::json;

#ifndef tojson_h
#define tojson_h

class ToJson{

public:
// Bumped when the .mx layout changes. Every version is additive, so a reader of any
// older version still loads a newer file; this describes the file rather than gating
// loading it. See docs/MX_FORMAT.md for the full layout.
//   1  [x, y, w, h] locations
//   2  a fifth location element: elevation
//   3  per-tile "flags" and "collision" - what a tile means, not just how it looks
static constexpr int kMXFormatVersion = 3;

ToJson();
~ToJson();

void SaveToJson(string, map<string, vector<GameTile *>>, map<string, TileType>);  //Function used to write json to
void ImportMX(string);
void ExportMX(map<string, vector<GameTile *>>, string);
void SaveMXProject(map<string, bool>, string);
json LoadMXProject(string);

// Moved to public due to MX importing issues
json json_blocks;
json json_pr;


};

#endif