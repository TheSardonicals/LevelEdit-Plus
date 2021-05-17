#include "tojson.h"


ToJson::ToJson(){
    this->json_blocks = {
        {"name", ""},
        {"Tiles", {}}
    };
}

ToJson::~ToJson(){}

void ToJson::ImportJson(){}

void ToJson::SaveToJson(string name, map<string, vector<GameTile *>> tile_cache){
    
    json_blocks["name"] = name;

    for (auto it : tile_cache){
        for (auto tile : it.second){
            auto it_tiles = json_blocks["Tiles"].find(tile->name);
            if (it_tiles != json_blocks["Tiles"].end() == true){
                json_blocks["Tiles"][tile->name].push_back({tile->x, tile->y, tile->w, tile->h});
            }else{
                json_blocks["Tiles"][tile->name] = {{tile->x, tile->y, tile->w, tile->h}};
            }
        }
    }

}


void ToJson::ExportJson(string filename){

    fstream jsonfile;
    string export_dir = "../bin/" + filename + ".json";

    jsonfile.open(export_dir);

    jsonfile << setw(4) << json_blocks;
    
    jsonfile.close();
}
