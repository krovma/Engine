#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <sstream>

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

////////////////////////////////
std::vector<std::string> Split(const char* cstring, char delimiter /*= ' '*/, bool reserveDelimiter /*= false*/)
{
	std::vector<std::string> result;
	result.push_back("");
	int resultIndex = 0;
	for (int i = 0; cstring[i] != 0; ++i) {
		if (cstring[i] != delimiter) {
			result[resultIndex] += cstring[i];
		} else {
			if (reserveDelimiter) {
				result[resultIndex] += cstring[i];
			}
			result.push_back("");
			++resultIndex;
		}
	}
	return result;
}

////////////////////////////////
int LoadTextFileToString(const std::string& filePath, std::string& out_string)
{
	std::ifstream file;
	std::stringstream fileStream;
	try {
		file.open(filePath);
		fileStream << file.rdbuf();
		file.close();
	}
	catch (std::ifstream::failure error) {
		ERROR_AND_DIE(Stringf("Failed on reading file %s\n%s\n", filePath, error.what()));
		return -1;
	}
	out_string = fileStream.str();
	return out_string.length();
}




