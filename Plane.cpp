#include "pch.h"
#include "Plane.h"
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
glm::vec3 planeColor = glm::vec3(0.5f, 0.5f, 0.5f); 

Plane::Plane() {
    // Bind VAO
    vao.Bind();

    // Create and bind VBO
    vbo = new VBO(vertices.data(), vertices.size() * sizeof(GLfloat));

    // Create and bind EBO
    ebo = new EBO(indices.data(), indices.size() * sizeof(GLuint));

    // Link attributes
    vao.LinkAttrib(*vbo, 0, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)0);             
    vao.LinkAttrib(*vbo, 1, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); 
    vao.LinkAttrib(*vbo, 2, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

  
    vao.Unbind();
    vbo->Unbind();
    ebo->Unbind();
}


Plane::~Plane() {
   
    delete vbo;
    delete ebo;
}

void Plane::Render(Shader& shader, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation) {
 
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (rotation != glm::vec3(0.0f)) {
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    model = glm::scale(model, scale);

  
    shader.Activate();
    GLint modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.Unbind();
}



