#include"pch.h"
#include "Camera.h"

Camera::Camera()
{
}

// Constructor
Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : m_Position(position), m_WorldUp(up), m_Zoom(45.0f)
{
    m_Front = glm::normalize(target - position);
    m_Yaw = -90.0f; // Default facing negative Z
    m_Pitch = 0.0f;
    UpdateCameraVectors();
}

// Return the view matrix
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

// Get current zoom
float Camera::GetZoom() const
{
    return m_Zoom;
}

// Handle keyboard input
void Camera::ProcessKeyboard(int key, float deltaTime)
{
    const float velocity = 2.5f * deltaTime;
    switch (key)
    {
    case VK_UP: // Zoom in
        m_Zoom -= 1.0f;
        if (m_Zoom < 1.0f) m_Zoom = 1.0f;
        break;
    case VK_DOWN: // Zoom out
        m_Zoom += 1.0f;
        if (m_Zoom > 45.0f) m_Zoom = 45.0f;
        break;
    case VK_LEFT: // Rotate left
        m_Yaw -= 1.0f;
        UpdateCameraVectors();
        break;
    case VK_RIGHT: // Rotate right
        m_Yaw += 1.0f;
        UpdateCameraVectors();
        break;
    default:
        break;
    }
}

// Handle mouse movement
void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
    const float sensitivity = 0.1f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;

    m_Yaw += deltaX;
    m_Pitch += deltaY;

    // Constrain pitch to avoid flipping
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    UpdateCameraVectors();
}

// Handle mouse wheel for zoom
void Camera::ProcessMouseWheel(float deltaZoom)
{
    m_Zoom -= deltaZoom;
    if (m_Zoom < 1.0f) m_Zoom = 1.0f;
    if (m_Zoom > 45.0f) m_Zoom = 45.0f;
}

// Handle translation (dragging)
void Camera::Translate(float deltaX, float deltaY)
{
    const float sensitivity = 0.01f;
    m_Position += m_Right * deltaX * sensitivity;
    m_Position -= m_Up * deltaY * sensitivity;
}

// Update the camera's vectors based on yaw and pitch
void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Recalculate right and up vectors
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
