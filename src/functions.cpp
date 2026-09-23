#include "functions.h"

#include <cctype>

void ShowError(char* title, string message, string log, bool show_sdl_error){
    string error_string = "";
    if (show_sdl_error){
        error_string = SDL_GetError();
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, (message + error_string).c_str(), NULL);
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, (log + error_string).c_str());
}

vector<string> Split(string const & word, char delim){
    vector<string> strings;
    stringstream s(word);
    string w;
    while (getline(s, w, delim)){
        strings.push_back(w);
    }
    
    return strings;
}

map<string, string> Get_Resources(map<string, string> tiles){
    map<string, string> resources;

    for (map<string, string>::iterator t = tiles.begin(); t != tiles.end(); ++t){

    }

    return resources;
}

string FileExtension(string const & path){
    size_t slash = path.find_last_of("/\\");
    size_t dot = path.find_last_of('.');
    if (dot == string::npos || (slash != string::npos && dot < slash)){
        return "";
    }

    string extension = path.substr(dot);
    for (char & c : extension){
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return extension;
}

bool IsImageFile(string const & path){
    string extension = FileExtension(path);
    // .bmp is read by SDL, the rest by stb_image. Anything else in the folder -
    // a licence file, a stray .aseprite - is skipped rather than loaded and
    // reported as broken.
    static const char * kImages[] = {".bmp", ".png", ".jpg", ".jpeg", ".tga", ".gif"};
    for (const char * image : kImages){
        if (extension == image) return true;
    }
    return false;
}

string AssetKey(string const & path){
    string key = path;

    // Windows hands back backslashes; the key is always written with forward
    // slashes so a map saved on either platform reads the same on the other.
    for (char & c : key){
        if (c == '\\') c = '/';
    }

    const string root = "resources/";
    if (key.compare(0, root.size(), root) == 0){
        key.erase(0, root.size());
    }

    string extension = FileExtension(key);
    if (!extension.empty()){
        key.erase(key.size() - extension.size());
    }
    return key;
}

string AssetFileName(string const & key, string const & source_path){
    string name = key;
    for (char & c : name){
        if (c == '/') c = '_';
    }

    string extension = FileExtension(source_path);
    return name + (extension.empty() ? ".bmp" : extension);
}

map<string, string> GetPaths(){
  map<string, string> paths;
  string filepath = "resources/";

  // Recursive, so a folder of art copied in from another project keeps its own
  // shape instead of having to be flattened by hand first. Non-images are
  // skipped: an imported folder usually carries a few files that are not art.
  for (auto &p : fs::recursive_directory_iterator(filepath)){
      string path = p.path().string();
      if (!fs::is_regular_file(p.path()) || !IsImageFile(path)) continue;
      paths[AssetKey(path)] = path;
  }
  return paths;
}