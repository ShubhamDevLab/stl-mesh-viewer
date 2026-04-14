#ifndef CAMERA_H
#define CAMERA_H
#include<Windows.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

class Camera
{
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

    // Getters
    glm::mat4 GetViewMatrix() const;
    float GetZoom() const;

    // Methods for handling input
    void ProcessKeyboard(int key, float deltaTime);
    void ProcessMouseMovement(float deltaX, float deltaY);
    void ProcessMouseWheel(float deltaZoom);

    // Translation (dragging)
    void Translate(float deltaX, float deltaY);

private:
    // Camera attributes
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // Rotation and zoom
    float m_Yaw;
    float m_Pitch;
    float m_Zoom;

    // Update internal vectors
    void UpdateCameraVectors();
};

#endif // CAMERA_H
