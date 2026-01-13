#version 450

// ============================================================
// PHONG SHADING VERTEX SHADER with SHADOW SUPPORT
// ============================================================

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightSpaceMatrix;  // NEW: For shadow mapping
    vec3 viewPos;
    float time;
    vec3 lightPos;
    float lightIntensity;
    vec3 lightColor;
    float ambientStrength;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;
layout(location = 4) out vec4 fragPosLightSpace;  // NEW: Position in light space

void main() {
    // Transform to world space
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
    
    // Transform normal to world space
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    fragTexCoord = inTexCoord;
    
    // Transform to light space for shadow mapping
    fragPosLightSpace = ubo.lightSpaceMatrix * worldPos;
}