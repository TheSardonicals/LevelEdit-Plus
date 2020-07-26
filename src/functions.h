#pragma once
#include "headers.h"

#ifndef functions_h
#define functions_h

void ShowError(char * title, string message, string log, bool show_sdl_error);

vector<string> split(string const & word, char delim = ' ');
vector<string> Get_Resources();
vector<string> Get_Paths();

#endif /* functions_h */
