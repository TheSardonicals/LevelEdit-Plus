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

void init(json*);

void AddToJson(json*, string, GameTile&);       //Function used to append to json object
void SaveToJson(json*, string);             //Function used to write json to 
void ReadJson(json*);

private: 

json json_blocks;
json json_cache;

};

#endif