#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "DayNightCycle.h"

/**
 * @brief Manages texture loading and GPU resources
 * 
 * Role: Handle texture lifecycle and provide texture data to shaders
 * Responsibilities:
 * - Load images from disk (PNG, JPG, etc.)
 * - Create Vulkan images, views, and samplers
 * - Manage texture memory efficiently
 * - Provide descriptors for shader binding
 * 
 * Design Notes:
 * - Single Responsibility: Only handles textures
 * - Low coupling: Receives Vulkan handles via init()
 * - Supports future texture caching via path lookup
 */
class TextureManager {
public:
    /**
     * @brief Texture resource bundle
     */
    struct Texture {
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        uint32_t width = 0;
        uint32_t height = 0;
        std::string path;
    };

    TextureManager() = default;
    ~TextureManager() = default;

    // Non-copyable
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    /**
     * @brief Initialize with Vulkan handles
     * Must be called before any texture operations
     */
    void init(VkDevice device, VkPhysicalDevice physicalDevice, 
              VkCommandPool commandPool, VkQueue graphicsQueue);

    /**
     * @brief Load a texture from file
     * @param filepath Path to image file (PNG, JPG, BMP, TGA)
     * @return Index of loaded texture, or -1 on failure
     */
    int32_t loadTexture(const std::string& filepath);

    /**
     * @brief Create a procedural texture (e.g., sand pattern)
     * @param width Texture width in pixels
     * @param height Texture height in pixels
     * @param data RGBA pixel data
     * @return Index of created texture
     */
    int32_t createTexture(uint32_t width, uint32_t height, 
                          const std::vector<uint8_t>& data);

    /**
     * @brief Generate a procedural sand texture
     * @param width Texture width
     * @param height Texture height
     * @return Index of created texture
     */
    int32_t generateSandTexture(uint32_t width, uint32_t height);

    /**
     * @brief Get texture by index
     */
    const Texture& getTexture(int32_t index) const;

    /**
     * @brief Get descriptor image info for shader binding
     */
    VkDescriptorImageInfo getDescriptorInfo(int32_t index) const;

    /**
     * @brief Clean up all texture resources
     */
    void cleanup();

    /**
     * @brief Get number of loaded textures
     */
    size_t getTextureCount() const { return m_textures.size(); }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;

    std::vector<Texture> m_textures;
    std::unordered_map<std::string, int32_t> m_textureCache;

    // Helper functions
    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage& image,
                     VkDeviceMemory& memory);

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkImageView createImageView(VkImage image, VkFormat format);
    VkSampler createSampler();
};