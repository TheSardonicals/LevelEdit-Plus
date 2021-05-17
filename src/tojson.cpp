#include "tojson.h"


ToJson::ToJson(){}

ToJson::~ToJson(){}

void AddToJson(json* j, string n, GameTile& tile){
    
}


void SaveToJson(json* j){

    fstream jsonfile;
    string f = "../bin/gametiles.json";

    jsonfile.open(f);

    jsonfile << setw(4) << j;
    
    jsonfile.close();


}
