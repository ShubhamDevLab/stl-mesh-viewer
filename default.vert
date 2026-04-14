#version 330 core

layout(location = 0) in vec3 aPosition;  // Position
layout(location = 1) in vec3 aNormal;    // Normal
layout(location = 2) in vec3 aColor;  

out vec3 fragPosition;  // Position passed to fragment shader
out vec3 fragNormal;    // Normal passed to fragment shader
out vec3 fragColor; 
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 overrideColor; // Color for wireframe mode

void main()
{
    fragPosition = vec3(model * vec4(aPosition, 1.0));  // Transform the position
    fragNormal = mat3(transpose(inverse(model))) * aNormal;  // Transform the normal
    fragColor = aColor;
    gl_Position = proj * view * vec4(fragPosition, 1.0);
}
