#pragma once
#include "json.hpp"
#include "gametile.h"
#include <iostream>
#include <experimental/filesystem>
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
void ImportMX();
void ExportMX(map<string, vector<GameTile *>>, string) ;
void SaveMXProject();
void LoadMXProject();



private:

json json_blocks;
json json_pr;


};

#endif