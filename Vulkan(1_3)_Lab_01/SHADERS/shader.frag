#version 450

// ============================================================
// PHONG SHADING with SHADOW MAPPING
// ============================================================

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec4 fragPosLightSpace;

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
    
    // Detect object type based on position
    float distFromCenter = length(fragPos);
    bool isGlobe = (distFromCenter > 95.0 && distFromCenter < 105.0);
    bool isGround = (abs(fragPos.y) < 1.0) && (normal.y > 0.9);
    
    vec3 surfaceColor;
    if (isGround) {
        vec4 texColor = texture(texSampler, fragTexCoord);
        surfaceColor = texColor.rgb;
    } else {
        surfaceColor = fragColor;
    }
    
    // ===== LIGHTING =====
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    
    // Ambient
    vec3 ambient = ubo.ambientStrength * ubo.lightColor;
    
    // Diffuse with special handling for globe
    float diff;
    if (isGlobe) {
        // Wrap lighting for translucent globe
        float wrap = 0.5;
        diff = max(0.0, (dot(normal, lightDir) + wrap) / (1.0 + wrap));
        
        // Subsurface scattering approximation
        float backLight = max(0.0, dot(-normal, lightDir)) * 0.3;
        diff += backLight;
    } else {
        diff = max(dot(normal, lightDir), 0.0);
    }
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    
    // Specular
    float specularStrength = isGround ? 0.1 : (isGlobe ? 0.3 : 0.5);
    float shininess = isGround ? 4.0 : (isGlobe ? 64.0 : 32.0);
    
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * ubo.lightColor;
    
    // Calculate shadow (only for ground, not globe)
    float shadow = 0.0;
    if (isGround) {
        shadow = calculateShadow(fragPosLightSpace, normal, lightDir);
    }
    
    // Apply shadow to diffuse and specular (not ambient)
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    vec3 result = lighting * surfaceColor;
    
    outColor = vec4(result, 1.0);
}