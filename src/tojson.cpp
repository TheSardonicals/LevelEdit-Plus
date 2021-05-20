#include "tojson.h"


ToJson::ToJson(){
    this->json_blocks = {
        {"name", ""},
        {"tiles", {}}
    };
}

ToJson::~ToJson(){}

void ToJson::ImportJson(){}

void ToJson::SaveToJson(string name, map<string, vector<GameTile *>> tile_cache){
    
    json_blocks["name"] = name;

    for (auto it : tile_cache){
        for (auto tile : it.second){
            auto it_tiles = json_blocks["tiles"].find(tile->name);
            if (it_tiles != json_blocks["tiles"].end() == true){
                json_blocks["tiles"][tile->name].push_back({tile->x, tile->y, tile->w, tile->h});
            }else{
                json_blocks["tiles"][tile->name] = {{tile->x, tile->y, tile->w, tile->h}};
            }
        }
    }

}


void ToJson::ExportJson(string filename){

    ofstream jsonfile;
    string export_dir = "../bin/" + filename + ".mxpr";
    //string export_dir = "../bin/testjson.json";

    jsonfile.open(export_dir, ios::out);
    jsonfile << setw(4) << json_blocks;
    jsonfile.close();
}
