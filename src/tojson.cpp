#include "tojson.h"
#include <fstream>
#include <iostream>


ToJson::ToJson(){
    this->json_blocks = {
        {"name", ""},
        {"tiles", {}}
    };
}

ToJson::~ToJson(){}

void ToJson::ImportMX(string filepath){
    string fp = "exports/" + filepath + "/";
    string assets_fp = "exports/assets";
    string mx_file = fp + filepath + ".mx";

    // If the Json with the blocks is not empty, run the constructor to reset
    if (!json_blocks.empty()){
        ToJson();
    }

    std::ifstream f(mx_file);
    this->json_blocks = json::parse(f);

    // Uncomment to see output of the json parse
    //cout << json_blocks.dump() << endl;
    
}

void ToJson::SaveToJson(string name, map<string, vector<GameTile *>> tile_cache){
    if (this->json_blocks["name"] != name){
        this->json_blocks["name"] = name;
    }
    
    // Locations are [x, y, w, h, height]. The height is a fifth element rather than a
    // new object field on purpose: readers that only know the original four-element
    // form index 0..3 and ignore the rest, so maps saved here still load in anything
    // built against the old format, just flat.
    this->json_blocks["formatVersion"] = kMXFormatVersion;

    for (auto it : tile_cache){
        for (auto tile : it.second){
            auto it_tiles = json_blocks["tiles"].find(tile->name);
            if (it_tiles != json_blocks["tiles"].end() == true){
                this->json_blocks["tiles"][tile->name]["locations"].push_back({tile->x, tile->y, tile->w, tile->h, tile->elevation});

            }else{
                this->json_blocks["tiles"][tile->name]["filepath"] = "exports/" + name + "/assets/" + tile->name + ".bmp";
                this->json_blocks["tiles"][tile->name]["locations"] = {{tile->x, tile->y, tile->w, tile->h, tile->elevation}};
            }
        }
    }

}

//TODO @isaboll1, @Dexsidius: Create a 'SaveMXPProject' function and 'LoadMXProject' function, and modify the class
//                            to support a json fomat specific to project files. use vscode as a reference perhaps.

// REMINDER: Exporting the files need to be saved as ".mx" files, because they export with simple directories,
// and in turn are expected to be supported outside of the editor, while ".mxpr" files are relative to the editor, and
// contain directory locations relative to the project that has to be saved. I think the Json format for ".mxpr" would be different to reflect this.
void ToJson::ExportMX(map<string, vector<GameTile *>> tile_cache, string filename = "default"){
    // create variables to be used for necessary paths, and create 'export' directory if it does not exist;
    string directory_path = "exports/"+ filename;
    experimental::filesystem::create_directories(directory_path.c_str());
    string export_dir = directory_path +"/" + filename+ ".mx";
    string tile_dir = directory_path + "/assets";
    experimental::filesystem::create_directories(tile_dir);
    
    //serialize the json string.
    string json_serialized = json_blocks.dump(4);
    
    // Copying the tiles in the tile cache from the editor resources to project folder.
    //
    // One copy per tile type rather than per placement: every placement of a tile
    // shares the same source .bmp, so copying per placement just re-copies the same
    // file over and over. The destination filename has to match what SaveToJson wrote
    // into "filepath", which is built from the tile's own name.
    //
    // copy() onto a directory is not used here: it reports "File exists" against an
    // existing destination directory on some standard library versions, so the export
    // silently produced an assets folder with nothing in it. copy_file() with an
    // explicit destination path does what was meant.
    vector<string> copied;
    int copy_failures = 0;

    for (auto it : tile_cache){
        for (auto tile : it.second){
            if (find(copied.begin(), copied.end(), tile->name) != copied.end()){
                continue;
            }
            copied.push_back(tile->name);

            string destination = tile_dir + "/" + tile->name + ".bmp";

            // Re-exporting a project should not trip over the assets a previous export
            // already put there.
            if (experimental::filesystem::exists(destination)){
                continue;
            }

            error_code error;
            experimental::filesystem::copy_file(tile->filepath, destination, error);

            if (error){
                // A tile whose art never made it across leaves an export the game
                // cannot load, so this gets reported rather than quietly skipped.
                cout << "couldn't copy " << tile->filepath << " -> " << destination
                     << ": " << error.message() << endl;
                copy_failures++;
            }
        }
    }

    if (copy_failures > 0){
        string asset_error = to_string(copy_failures) +
                             " tile image(s) could not be copied into " + tile_dir +
                             ". The exported map will be missing art.";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "LevelEdit++ : Asset Export Warning",
                                 asset_error.c_str(), NULL);
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

void ToJson::SaveMXProject(map<string, bool> bool_state, string project_name = "default"){
    string directory_path = "projects/" + project_name;
    experimental::filesystem::create_directories(directory_path.c_str());
    string export_dir = directory_path + "/" + project_name + ".mxpr";

    //MXPR files will hold the booleans and UI statuses and settings
    //Code here
    json_pr["project_name"] = project_name;
    
    for (auto state: bool_state){
        json_pr["states"][state.first] = state.second;
    }

    //MXPR serialization 
    string json_serialized = json_pr.dump(4);

    ofstream mxpr_file(export_dir.c_str());
    if (mxpr_file.is_open()){
        mxpr_file << json_serialized << "\n";
        mxpr_file.close();
    } else{
        string file_error = "couldn't write " + export_dir;
        cout << file_error;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "LevelEdit++ File Writing Error", file_error.c_str(), NULL);
    }

}

json ToJson::LoadMXProject(string filepath){
    
}