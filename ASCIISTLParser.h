#ifndef ASCII_STL_PARSER_H
#define ASCII_STL_PARSER_H

#include "STLLoader.h"
#include <vector>
#include <fstream>

class ParserASCIISTL
{
public:
	std::vector<Triangle> parseASCIISTL(std::ifstream& file);
};
#endif 
