#version 450

// ============================================================
// GOURAUD SHADING - Fragment Shader
// ============================================================
// Simply outputs the interpolated pre-lit color from vertex shader
// The lighting was already computed per-vertex
// ============================================================

layout(location = 0) in vec3 fragLitColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // Output interpolated lit color (no per-pixel lighting calculation)
    outColor = vec4(fragLitColor, 1.0);
}