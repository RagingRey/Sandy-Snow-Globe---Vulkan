#version 450

// ============================================================
// GOURAUD SHADING (Per-Vertex Lighting) with SHADOW SUPPORT
// ============================================================

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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 fragLitColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragPosLightSpace;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragPos;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    
    gl_Position = ubo.proj * ubo.view * worldPos;
    
    vec3 normal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
    fragNormal = normal;
    
    // Detect globe: vertices on a sphere centered at origin with radius ~100
    float distFromCenter = length(fragPos);
    bool isGlobe = (distFromCenter > 95.0 && distFromCenter < 105.0);
    
    // ===== GOURAUD LIGHTING (per-vertex) =====
    vec3 ambient = ubo.ambientStrength * ubo.lightColor;
    
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // For globe: use wrap lighting for translucent appearance
    if (isGlobe) {
        // Wrap lighting: shifts the terminator and softens the falloff
        float wrap = 0.5;  // How much light wraps around (0 = none, 1 = full)
        diff = max(0.0, (dot(normal, lightDir) + wrap) / (1.0 + wrap));
        
        // Add subsurface scattering approximation for translucency
        float backLight = max(0.0, dot(-normal, lightDir)) * 0.3;
        diff += backLight;
    }
    
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    
    // Specular (reduced for globe to emphasize translucency)
    float specularStrength = isGlobe ? 0.3 : 0.5;
    float shininess = isGlobe ? 64.0 : 32.0;
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * ubo.lightColor;
    
    // Store pre-lit color (shadow will be applied in fragment shader)
    fragLitColor = (ambient + diffuse + specular) * inColor;
    fragTexCoord = inTexCoord;
    
    // Light space position for shadow mapping
    fragPosLightSpace = ubo.lightSpaceMatrix * worldPos;
}