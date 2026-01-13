#version 450

// ============================================================
// SHADOW MAP VERTEX SHADER
// ============================================================
// Renders scene from light's perspective to generate depth map.
// Only outputs position - no color/normal needed for depth-only pass.
// ============================================================

layout(binding = 0) uniform ShadowUBO {
    mat4 lightSpaceMatrix;
    mat4 model;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;      // Unused but must match vertex layout
layout(location = 2) in vec2 inTexCoord;    // Unused
layout(location = 3) in vec3 inColor;       // Unused

void main() {
    gl_Position = ubo.lightSpaceMatrix * ubo.model * vec4(inPosition, 1.0);
}