#version 450

// ============================================================
// PHONG SHADING (Per-Pixel Lighting)
// ============================================================
// Lighting calculations performed per-fragment for accurate results
// Best for: Low-polygon meshes where interpolation artifacts are visible
// 
// Lab Book Note: Phong shading interpolates normals across the surface,
// then calculates lighting per-pixel, producing smooth highlights even
// on coarse geometry. More expensive than Gouraud but higher quality.
// ============================================================

// Inputs from vertex shader (interpolated across triangle)
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

// Output color
layout(location = 0) out vec4 outColor;

// Uniform buffer - matches C++ UniformBufferObject struct
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

void main() {
    // Re-normalize the interpolated normal (interpolation changes length)
    vec3 normal = normalize(fragNormal);
    
    // ===== AMBIENT COMPONENT =====
    // Simulates indirect light bouncing in the environment
    // Prevents completely black areas in shadows
    vec3 ambient = ubo.ambientStrength * ubo.lightColor;
    
    // ===== DIFFUSE COMPONENT (Lambertian Reflection) =====
    // Light intensity proportional to angle between normal and light direction
    // max() prevents negative values when surface faces away from light
    vec3 lightDir = normalize(ubo.lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    
    // ===== SPECULAR COMPONENT (Phong Reflection Model) =====
    // Creates shiny highlights based on view angle relative to reflection
    // shininess controls highlight tightness (higher = smaller, sharper)
    float specularStrength = 0.5;
    float shininess = 32.0;
    
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * ubo.lightColor;
    
    // ===== FINAL COLOR =====
    // Combine all lighting components with surface color
    vec3 result = (ambient + diffuse + specular) * fragColor;
    
    // Output with full opacity
    outColor = vec4(result, 1.0);
}