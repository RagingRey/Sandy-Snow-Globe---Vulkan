#pragma once

#include <glm/glm.hpp>

// ============================================================
// PARTICLE DATA STRUCTURE
// ============================================================
// Lightweight structure for individual particle state.
// Designed for cache-friendly iteration in particle systems.
// ============================================================

struct Particle {
    glm::vec3 position;     // World position
    glm::vec3 velocity;     // Movement per second
    glm::vec4 color;        // RGBA with alpha for fade
    float life;             // Remaining life (seconds)
    float maxLife;          // Initial life for interpolation
    float size;             // Billboard size
    
    // Returns true if particle is still alive
    bool isAlive() const { return life > 0.0f; }
    
    // Get normalized age (0.0 = just born, 1.0 = about to die)
    float getAge() const { 
        return maxLife > 0.0f ? 1.0f - (life / maxLife) : 1.0f; 
    }
};

// GPU-friendly vertex for particle rendering (billboard quad)
struct ParticleVertex {
    glm::vec3 position;     // Center position
    glm::vec4 color;        // Color with alpha
    glm::vec2 texCoord;     // UV for texture/procedural
    float size;             // Particle size
    
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(ParticleVertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }
    
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributes{};
        
        // Position
        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(ParticleVertex, position);
        
        // Color
        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[1].offset = offsetof(ParticleVertex, color);
        
        // TexCoord
        attributes[2].binding = 0;
        attributes[2].location = 2;
        attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[2].offset = offsetof(ParticleVertex, texCoord);
        
        // Size
        attributes[3].binding = 0;
        attributes[3].location = 3;
        attributes[3].format = VK_FORMAT_R32_SFLOAT;
        attributes[3].offset = offsetof(ParticleVertex, size);
        
        return attributes;
    }
};