#include"pch.h"
#include "STLLoader.h"
#include "ASCIISTLParser.h"
#include "BinarySTLParser.h"
#include <fstream>
#include <iostream>

// Helper function to determine if the file is ASCII or binary
bool isASCII(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    std::string firstWord;
    file >> firstWord;
    file.close();
    return firstWord == "solid";  // ASCII STL starts with "solid"
}

STLLoader::STLLoader() 
{
}
void STLLoader::calculateSurfaceAreaAndVolume(const std::vector<Triangle>& triangles,float& totalArea, float& totalVolume) const
{
    totalArea = 0.0f;
    totalVolume = 0.0f;
    for (const auto& triangle : triangles) {
        totalArea += triangle.area();
        totalVolume += triangle.volume();
    }
}

std::vector<Triangle> STLLoader::loadSTL(const std::string& filename)
{
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open STL file: " << filename << std::endl;
            return {};
        }
    
        //std::vector<Triangle> triangles;
    
        if (isASCII(filename)) {
            file.close();  // Close binary stream and reopen in text mode
            std::ifstream asciiFile(filename);
            ParserASCIISTL parserASCII;
            triangles = parserASCII.parseASCIISTL(asciiFile);
        }
        else {
            ParserBinarySTL parserBinary;
            triangles = parserBinary.parseBinarySTL(file);
        }
    
        return triangles;
}

const std::vector<Triangle>& STLLoader::getTriangles() const {
    return triangles;
}