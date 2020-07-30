#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <experimental/filesystem>
#include <SDL2/SDL.h>

using namespace std;
namespace fs = std::experimental::filesystem;

#ifndef functions_h
#define functions_h

void ShowError(char * title, string message, string log, bool show_sdl_error);

// Split function returns a vector of each split char or item.
vector<string> Split(string const & word, char delim = ' ');
map<string, string> Get_Resources();
map<string, string> GetPaths();

#endif /* functions_h */
