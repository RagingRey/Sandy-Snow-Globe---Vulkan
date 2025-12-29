#version 450

// ============================================================
// PARTICLE VERTEX SHADER
// ============================================================
// Passes billboard vertex data to fragment shader.
// Billboarding is computed on CPU for simplicity.
// ============================================================

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
    float time;
    vec3 lightPos;
    float lightIntensity;
    vec3 lightColor;
    float ambientStrength;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inSize;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}