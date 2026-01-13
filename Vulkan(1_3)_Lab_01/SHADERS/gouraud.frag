#version 450

// ============================================================
// GOURAUD SHADING - Fragment Shader with SHADOW SUPPORT
// ============================================================
// Gouraud computes lighting per-vertex, so shadows are simplified.
// Only ground plane receives shadows to avoid globe self-shadowing.
// ============================================================

layout(location = 0) in vec3 fragLitColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragPosLightSpace;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightSpaceMatrix;
    vec3 viewPos;
    float time;
    vec3 lightPos;
    float lightIntensity;
    vec3 lightColor;
    float ambientStrength;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D shadowMap;

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 0.0;
    }
    
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    
    // Determine if this is ground plane (same logic as Phong shader)
    bool isGround = (abs(fragPos.y) < 1.0) && (normal.y > 0.9);
    
    vec3 result = fragLitColor;
    
    // Only apply shadows to ground plane (not globe or cacti)
    // This avoids the globe self-shadowing artifact
    if (isGround) {
        float shadow = calculateShadow(fragPosLightSpace, normal, lightDir);
        
        // Darken ground in shadow, but keep minimum ambient
        float shadowFactor = 1.0 - shadow * 0.6;
        result = fragLitColor * shadowFactor;
        
        // Apply ground texture
        vec4 texColor = texture(texSampler, fragTexCoord);
        result = result * texColor.rgb;
    }
    
    outColor = vec4(result, 1.0);
}