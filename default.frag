#version 330 core

in vec3 fragPosition;  // Position from vertex shader
in vec3 fragNormal;    // Normal from vertex shader
in vec3 fragColor; 

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 oppositeLightColor;
uniform vec3 oppositeLightPos;
uniform vec3 topLightPos;
uniform vec3 topLightColor;
uniform vec3 viewPos;  // Camera position

uniform bool isWireframe;      // Indicates if wireframe mode is enabled
uniform bool isSilhouette;     // Indicates if silhouette mode is enabled
uniform vec3 overrideColor;    // Override color for wireframe or silhouette  

// Spotlight parameters
uniform vec3  spotlightColor;
uniform vec3 spotLightPos;        // Spotlight position
uniform vec3 spotLightDir;        // Spotlight direction
uniform float spotLightInnerAngle; // Inner cone angle in radians
uniform float spotLightOuterAngle; // Outer cone angle in radians
uniform vec3 planeColor; // Plane color uniform
void main()
{
 if (isWireframe)
    {
        // Render only the wireframe color
        FragColor = vec4(overrideColor, 1.0f);
        return;
    }

    if (isSilhouette)
    {
        // Silhouette effect: calculate intensity based on view and normal
        vec3 viewDir = normalize(viewPos - fragPosition);
        float edgeIntensity = abs(dot(viewDir, fragNormal));

        if (edgeIntensity < 0.2)
        {
            FragColor = vec4(overrideColor, 1.0f); // Silhouette color (e.g., black)
            return;
        }
    }

    // Normalize the vectors to ensure they are unit vectors
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 viewDir = normalize(viewPos - fragPosition);

    // Ambient lighting
    float ambientStrength = 0.004f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor * 0.9f;

    // Specular lighting
    float specularStrength = 0.5f;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 64);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all lighting components
    vec3 light1Result = ambient + diffuse + specular;

    // Second light source
    vec3 oppositeLightDir = normalize(oppositeLightPos - fragPosition);
    vec3 ambient2 = ambientStrength * oppositeLightColor;
    float diff2 = max(dot(norm, oppositeLightDir), 0.0f);
    vec3 diffuse2 = diff2 * oppositeLightColor * 0.9f;
    vec3 reflectDir2 = reflect(-oppositeLightDir, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0f), 64);
    vec3 specular2 = specularStrength * spec2 * oppositeLightColor;
    vec3 light2Result = ambient2 + diffuse2 + specular2;

    // Top light source
    vec3 topLightDir = normalize(topLightPos - fragPosition);
    vec3 ambient3 = ambientStrength * topLightColor;
    float topdiff = max(dot(norm, topLightDir), 0.0f);
  
    vec3 diffuse3 = topdiff * topLightColor * 0.7f;
    vec3 reflectDir3 = reflect(-topLightDir, norm);
    float spec3 = pow(max(dot(viewDir, reflectDir3), 0.0f), 8);
    vec3 specular3 = specularStrength * spec3 * topLightColor;
    vec3 topLight = ambient3 + specular3 + diffuse3 ;

    /// Spotlight calculations:
    vec3 spotDir = normalize(spotLightPos - fragPosition); // Direction from fragment to spotlight
    float spotAngle = acos(dot(norm, spotDir)); // Angle between normal and spotlight direction

    // Check if the fragment is inside the spotlight's cone (inner and outer angles)
    float spotIntensity = 0.0f;
    if (spotAngle < spotLightInnerAngle)
    {
        // Full intensity if inside the inner cone
        spotIntensity = 1.0f;
    }
    else if (spotAngle < spotLightOuterAngle)
    {
        // Falloff between the inner and outer cone
        float falloff = 1.0f - (spotAngle - spotLightInnerAngle) / (spotLightOuterAngle - spotLightInnerAngle);
        spotIntensity = max(falloff, 0.0f);
    }
    float distance = length(fragPosition - spotLightPos);
float attenuation = 1.0 / (distance * distance);
//spotlightColor *= attenuation;

    // Apply spotlight color and intensity
    vec3 spotlightColor =( spotlightColor * spotIntensity ) ;

    // Final color: Combine all light results
    vec3 result = (light1Result + light2Result + topLight + spotlightColor) * fragColor;
    
    FragColor = vec4(result, 1.0f);
   

}
