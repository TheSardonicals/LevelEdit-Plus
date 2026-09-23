#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <experimental/filesystem>
#include <SDL3/SDL.h>

using namespace std;
namespace fs = std::experimental::filesystem;

#ifndef functions_h
#define functions_h

void ShowError(char * title, string message, string log, bool show_sdl_error);

// Split function returns a vector of each split char or item.
vector<string> Split(string const & word, char delim = ' ');
map<string, string> Get_Resources();
map<string, string> GetPaths();

// Lowercased extension including the dot (".png"), or "" when there is none.
string FileExtension(string const & path);

// True for the image formats the editor can load: .bmp through SDL, the rest
// through stb_image.
bool IsImageFile(string const & path);

// The name a tile is known by, from its path under resources/, without the
// extension and always with forward slashes: "resources/tiles/grass.png" gives
// "tiles/grass". Keeping the folder in the name is what lets a whole folder of
// art be dropped in from another project without two "grass" tiles colliding.
string AssetKey(string const & path);

// What that tile's image is called inside an export. The folders in a key
// become part of the filename, since an export keeps every image in one
// directory: "tiles/grass" with a .png source gives "tiles_grass.png".
string AssetFileName(string const & key, string const & source_path);

#endif /* functions_h */
