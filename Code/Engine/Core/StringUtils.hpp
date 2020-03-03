#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "EngineCommon.hpp"
//#define UNUSED(x) (void)(x)


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
/// assume \0 at end of string
std::vector<std::string> Split(const char* cstring, char delimiter = ' ', bool multiDelimitersAsOne = true, bool reserveDelimiter = false);

size_t LoadTextFileToString(const std::string& filePath, std::string& out_string);

template<typename T>
T MakeFromString(const std::string& str, const T& defaultValue)
{
	T val(defaultValue);
	val.SetFromText(str.c_str());
	return val;
}


std::string MakeFromString(const std::string& str, const std::string& defaultValue);
std::string MakeFromString(const std::string& str, const char*& defaultValue);
int MakeFromString(const std::string& str, const int& defaultValue);
bool MakeFromString(const std::string& str, const bool& defaultValue);
float MakeFromString(const std::string& str, const float& defaultValue);
char MakeFromString(const std::string& str, const char& defaultValue);