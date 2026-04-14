#ifndef STL_LOADER_H
#define STL_LOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp> 

struct Triangle {
    glm::vec3 vertex1;
    glm::vec3 vertex2;
    glm::vec3 vertex3;

    glm::vec3 normal1;
    glm::vec3 normal2;
    glm::vec3 normal3;

    float area() const {
        glm::vec3 v1 = vertex2 - vertex1;
        glm::vec3 v2 = vertex3 - vertex1;
        return 0.5f * glm::length(glm::cross(v1, v2));
    }

    // Calculate the volume contribution of the triangle (tetrahedron formed with origin)
    float volume() const {
        return (1.0f / 6.0f) * glm::dot(vertex1, glm::cross(vertex2, vertex3));
    }
};

class STLLoader
{
public:
    STLLoader();
    std::vector<Triangle> loadSTL(const std::string& filename);
    const std::vector<Triangle>& getTriangles() const;
    void calculateSurfaceAreaAndVolume(const std::vector<Triangle>& triangles,float& totalArea, float& totalVolume) const;

private:
  
    std::vector<Triangle> triangles;
};

#endif // STL_LOADER_H
