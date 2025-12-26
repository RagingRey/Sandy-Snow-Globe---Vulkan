#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

/**
 * @brief Vertex structure for rendering with full attribute support
 * 
 * Supports position, normal, texture coordinates, and color.
 * Required for both Gouraud (per-vertex) and Phong (per-pixel) shading.
 * 
 * Design Notes:
 * - Aligned for efficient GPU access
 * - Static methods provide Vulkan binding descriptions
 * - Extensible for future attributes (tangents, etc.)
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;

    bool operator==(const Vertex& other) const {
        return position == other.position && 
               normal == other.normal && 
               texCoord == other.texCoord &&
               color == other.color;
    }

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        
        // Position at location 0
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        // Normal at location 1
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        // Texture coordinate at location 2
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        // Color at location 3
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

// Hash function for Vertex (needed for std::unordered_map deduplication)
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(const Vertex& vertex) const {
            size_t h1 = hash<float>()(vertex.position.x) ^ (hash<float>()(vertex.position.y) << 1);
            size_t h2 = hash<float>()(vertex.position.z) ^ (hash<float>()(vertex.normal.x) << 1);
            size_t h3 = hash<float>()(vertex.normal.y) ^ (hash<float>()(vertex.normal.z) << 1);
            size_t h4 = hash<float>()(vertex.texCoord.x) ^ (hash<float>()(vertex.texCoord.y) << 1);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}