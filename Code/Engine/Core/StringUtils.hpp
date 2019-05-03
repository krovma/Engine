#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
/// assume \0 at end of string
std::vector<std::string> Split(const char* cstring, char delimiter = ' ', bool multiDelimitersAsOne = true, bool reserveDelimiter = false);

size_t LoadTextFileToString(const std::string& filePath, std::string& out_string);