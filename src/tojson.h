#pragma once
#include "json.hpp"
#include "gametile.h"
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
// Bumped when the .mx layout changes. 1 was [x, y, w, h] locations with no height;
// 2 adds the height as a fifth element. Readers of either version can load a v2 file,
// so this is here to describe the file rather than to gate loading it.
static constexpr int kMXFormatVersion = 2;

ToJson();
~ToJson();

void SaveToJson(string, map<string, vector<GameTile *>>);             //Function used to write json to 
void ImportMX(string);
void ExportMX(map<string, vector<GameTile *>>, string);
void SaveMXProject(map<string, bool>, string);
json LoadMXProject(string);

// Moved to public due to MX importing issues
json json_blocks;
json json_pr;


};

#endif