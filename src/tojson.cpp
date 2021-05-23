#include "tojson.h"


ToJson::ToJson(){
    this->json_blocks = {
        {"name", ""},
        {"tiles", {}}
    };
}

ToJson::~ToJson(){}

void ToJson::ImportMX(){}

void ToJson::SaveToJson(string name, map<string, vector<GameTile *>> tile_cache){
    if (json_blocks["name"] != name){
        json_blocks["name"] = name;
    }
    
    for (auto it : tile_cache){
        for (auto tile : it.second){
            auto it_tiles = json_blocks["tiles"].find(tile->name);
            if (it_tiles != json_blocks["tiles"].end() == true){
                json_blocks["tiles"][tile->name]["locations"].push_back({tile->x, tile->y, tile->w, tile->h});
                
            }else{
                json_blocks["tiles"][tile->name]["filepath"] = "assets/" + tile->name + ".bmp";
                json_blocks["tiles"][tile->name]["locations"] = {{tile->x, tile->y, tile->w, tile->h}};
            }
        }
    }

}

//TODO @isaboll1, @Dexsidius: Create a 'SaveMXPProject' function and 'LoadMXProject' function, and modify the class
//                            to support a json fomat specific to project files. use vscode as a reference perhaps.

// REMINDER: Exporting the files need to be saved as ".mx" files, because they export with simple directories,
// and in turn are expected to be supported outside of the editor, while ".mxpr" files are relative to the editor, and
// contain directory locations relative to the project that has to be saved. I think the Json format for ".mxpr" would be different to reflect this.
// TODO @isaboll1, @Dexsidius: Renamethis to 'ExportMX', and have it also handle exporting all assets to the same directory.
void ToJson::ExportMX(map<string, vector<GameTile *>> tile_cache, string filename = "default"){
    // create variables to be used for necessary paths, and create 'export' directory if it does not exist;
    string directory_path = "exports/"+ filename;
    experimental::filesystem::create_directories(directory_path.c_str());
    string export_dir = directory_path +"/" + filename+ ".mx";
    string tile_dir = directory_path + "/assets";
    experimental::filesystem::create_directories(tile_dir);
    
    //serialize the json string.
    string json_serialized = json_blocks.dump(4);
    cout << json_serialized << endl;
    
    //Copying the tiles in the tile cache from the editor resources to project folder
    for (auto it : tile_cache){
        for(auto tile : it.second){
            try{
                experimental::filesystem::copy(tile->filepath, tile_dir);
            }
            //Error thrown when copying a file that already exists in the folder. ie. multples of lava puddle will throw error on second lava
            catch(experimental::filesystem::filesystem_error){
                break;
            }
        }
    }

    //create the file within the path, and output the json to it.
    ofstream mx_file(export_dir.c_str());
    if (mx_file.is_open()){
        mx_file << json_serialized << '\n';
        mx_file.close();
    }
    else {
        string file_error = "couldn't write " + export_dir;
        cout << file_error << endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Level Edit++ : File Writing Error!", file_error.c_str(), NULL);
    }
    
}


