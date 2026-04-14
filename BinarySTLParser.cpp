#include"pch.h"
#include "BinarySTLParser.h"
#include <iostream>

std::vector<Triangle>ParserBinarySTL::parseBinarySTL(std::ifstream& file) {
    std::vector<Triangle> triangles;

    // Skip the 80-byte header
    file.ignore(80);

    uint32_t triangleCount;
    file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));
    triangles.resize(triangleCount);

    for (uint32_t i = 0; i < triangleCount; ++i) {
        float normal[3], v1[3], v2[3], v3[3];

        file.read(reinterpret_cast<char*>(normal), sizeof(normal));
        file.read(reinterpret_cast<char*>(v1), sizeof(v1));
        file.read(reinterpret_cast<char*>(v2), sizeof(v2));
        file.read(reinterpret_cast<char*>(v3), sizeof(v3));

        triangles[i] = {
            {v1[0], v1[1], v1[2]},
            {v2[0], v2[1], v2[2]},
            {v3[0], v3[1], v3[2]}
        };
        // Assign the normal to each vertex
        triangles[i].normal1 = { normal[0], normal[1], normal[2] };
        triangles[i].normal2 = { normal[0], normal[1], normal[2] };
        triangles[i].normal3 = { normal[0], normal[1], normal[2] };

        file.ignore(2);  // Skip 2-byte attribute byte count
    }

    return triangles;
}
