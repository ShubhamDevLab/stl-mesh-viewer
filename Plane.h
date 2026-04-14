#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

class Plane {
public:
    Plane();
    ~Plane();

    void Render(Shader& shader, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation = glm::vec3(0.0f));

private:
    VAO vao;
    VBO* vbo;
    EBO* ebo;

    std::vector<GLfloat> vertices = {
        // Positions          // Normals           // Colors (RGB)
        -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,    0.8f, 0.8f, 0.8f, 
         1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,    0.8f, 0.8f, 0.8f, 
         1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,    0.8f, 0.8f, 0.8f, 
        -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,    0.8f, 0.8f, 0.8f 
    };


    std::vector<GLuint> indices = {
        0, 1, 2,
        0, 2, 3,
    };
};
