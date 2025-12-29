#version 450

// ============================================================
// PARTICLE FRAGMENT SHADER
// ============================================================
// Renders soft circular particles with procedural falloff.
// No texture required - generates soft circle mathematically.
// ============================================================

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // Distance from center of quad (0,0 to 1,1 -> -1,-1 to 1,1)
    vec2 center = fragTexCoord * 2.0 - 1.0;
    float dist = length(center);
    
    // Soft circular falloff
    float alpha = 1.0 - smoothstep(0.0, 1.0, dist);
    
    // Apply particle color with computed alpha
    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
    
    // Discard fully transparent pixels
    if (outColor.a < 0.01) {
        discard;
    }
}