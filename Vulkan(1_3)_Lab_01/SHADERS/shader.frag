#version 450

// ============================================================
// PHONG SHADING with SELECTIVE TEXTURE MAPPING
// ============================================================
// Texture is only applied to the ground plane (Y near 0)
// Globe and cacti use their vertex colors
// ============================================================

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

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

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    vec3 normal = normalize(fragNormal);
    
    // Determine if this fragment is on the ground plane
    // Ground plane is at Y = 0, with normal pointing up (0, 1, 0)
    // Use normal.y > 0.9 to detect horizontal surfaces facing up
    bool isGround = (abs(fragPos.y) < 1.0) && (normal.y > 0.9);
    
    vec3 surfaceColor;
    if (isGround) {
        // Sample sand texture for ground
        vec4 texColor = texture(texSampler, fragTexCoord);
        surfaceColor = texColor.rgb;
    } else {
        // Use vertex color for globe, cacti, etc.
        surfaceColor = fragColor;
    }
    
    // ===== LIGHTING =====
    // Ambient
    vec3 ambient = ubo.ambientStrength * ubo.lightColor;
    
    // Diffuse
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    
    // Specular - adjust based on material
    float specularStrength = isGround ? 0.1 : 0.5;  // Sand is matte, globe is shiny
    float shininess = isGround ? 4.0 : 32.0;
    
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * ubo.lightColor;
    
    // Combine
    vec3 result = (ambient + diffuse + specular) * surfaceColor;
    
    outColor = vec4(result, 1.0);
}