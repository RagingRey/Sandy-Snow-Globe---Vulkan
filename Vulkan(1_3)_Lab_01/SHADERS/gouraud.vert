#version 450

// ============================================================
// GOURAUD SHADING (Per-Vertex Lighting)
// ============================================================
// Lighting calculations performed at each vertex, then interpolated
// Best for: High-polygon meshes where vertex density is sufficient
// 
// Lab Book Note: Gouraud shading is computationally cheaper as lighting
// is calculated once per vertex rather than per fragment. However, it
// can produce visible artifacts on low-poly geometry, especially with
// specular highlights (Mach banding). Works well when vertices are dense.
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

// Vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

// Outputs to fragment shader
layout(location = 0) out vec3 fragLitColor;  // Pre-computed lit color
layout(location = 1) out vec2 fragTexCoord;

void main() {
    // Transform position to world space
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    vec3 fragPos = worldPos.xyz;
    
    // Final clip-space position
    gl_Position = ubo.proj * ubo.view * worldPos;
    
    // Transform normal to world space
    vec3 normal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
    
    // ===== LIGHTING CALCULATION (at vertex) =====
    
    // Ambient
    vec3 ambient = ubo.ambientStrength * ubo.lightColor;
    
    // Diffuse
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    
    // Specular (Phong)
    float specularStrength = 0.5;
    float shininess = 32.0;
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * ubo.lightColor;
    
    // Combine lighting with vertex color
    fragLitColor = (ambient + diffuse + specular) * inColor;
    fragTexCoord = inTexCoord;
}