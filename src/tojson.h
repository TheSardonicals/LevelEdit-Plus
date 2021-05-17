#pragma once
#include "json.hpp"
#include "gametile.h"

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using nlohmann::json;

#ifndef tojson_h
#define tojson_h

class ToJson{

public: 
ToJson();
~ToJson();

void SaveToJson(string, map<string, vector<GameTile *>>);             //Function used to write json to 
void ImportJson();
void ExportJson(string);

private:

json json_blocks;


};

#endif