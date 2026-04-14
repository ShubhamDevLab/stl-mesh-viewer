#ifndef BINARY_STL_PARSER_H
#define BINARY_STL_PARSER_H

#include "STLLoader.h"
#include <vector>
#include <fstream>


class ParserBinarySTL
{
public:
std::vector<Triangle> parseBinarySTL(std::ifstream& file);
};
#endif
