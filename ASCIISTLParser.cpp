#include "pch.h"
#include "ASCIISTLParser.h"
#include <sstream>
#include <iostream>

std::vector<Triangle>ParserASCIISTL::parseASCIISTL(std::ifstream& file) {
    std::vector<Triangle> triangles;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "facet") {
            float normal[3];
            std::string normalKeyword;
            iss >> normalKeyword >> normal[0] >> normal[1] >> normal[2];

            // Read and skip the "outer loop" line
            std::getline(file, line);

            Triangle triangle;
            for (int i = 0; i < 3; ++i) {
                std::getline(file, line);
                std::istringstream vertexStream(line);
                std::string vertexKeyword;
                float x, y, z;
                vertexStream >> vertexKeyword >> x >> y >> z;

                if (vertexKeyword != "vertex") {
                    std::cerr << "Error: Expected 'vertex' keyword, found: " << vertexKeyword << std::endl;
                    continue;
                }

                if (i == 0) triangle.vertex1 = { x, y, z };
                if (i == 1) triangle.vertex2 = { x, y, z };
                if (i == 2) triangle.vertex3 = { x, y, z };
            }

            // Assign the normal to each vertex
            triangle.normal1 = { normal[0], normal[1], normal[2] };
            triangle.normal2 = { normal[0], normal[1], normal[2] };
            triangle.normal3 = { normal[0], normal[1], normal[2] };

            triangles.push_back(triangle);

            // Skip "endloop" and "endfacet" lines
            std::getline(file, line);
            std::getline(file, line);
        }
    }

    return triangles;
}
