#pragma once

#include "Mesh.h"
#include "MeshGenerator.h"
#include <glm/glm.hpp>
#include <vector>

/**
 * @brief Procedural cactus generator for desert scene
 * 
 * Role: Generate and manage cactus geometry
 * Responsibilities:
 * - Create Saguaro-style cacti from primitives (cylinders/cones)
 * - Support variable sizes (spec: up to 20m tall)
 * - Provide mesh data for rendering
 * - Track growth state for environmental effects
 * 
 * Design Notes (for Lab Book):
 * - Composition over inheritance: uses MeshGenerator for primitives
 * - Single Responsibility: only handles cactus geometry/state
 * - High cohesion: all methods relate to cactus representation
 * - Low coupling: depends only on Mesh and MeshGenerator
 */
class Cactus {
public:
    /**
     * @brief Cactus configuration parameters
     */
    struct Config {
        glm::vec3 position{0.0f};      // World position
        float height = 5.0f;            // Main trunk height (meters)
        float trunkRadius = 0.5f;       // Main trunk radius
        int numArms = 2;                // Number of side arms (0-4)
        float armHeight = 0.6f;         // Arm height as fraction of trunk
        glm::vec3 color{0.2f, 0.6f, 0.2f}; // Green color
        uint32_t segments = 12;         // Cylinder segments (LOD)
    };

    Cactus() = default;
    explicit Cactus(const Config& config);

    /**
     * @brief Generate the cactus mesh from primitives
     * @return Combined mesh of trunk and arms
     */
    Mesh generateMesh() const;

    // Accessors
    glm::vec3 getPosition() const { return m_config.position; }
    float getHeight() const { return m_config.height; }
    bool isBurning() const { return m_isBurning; }
    float getGrowthFactor() const { return m_growthFactor; }

    // State modifiers (for environmental effects)
    void setBurning(bool burning) { m_isBurning = burning; }
    void setGrowthFactor(float factor) { m_growthFactor = factor; }
    void grow(float amount);

    // Configuration
    void setConfig(const Config& config) { m_config = config; }
    const Config& getConfig() const { return m_config; }

private:
    /**
     * @brief Generate a single arm attached to trunk
     * @param attachHeight Height on trunk where arm attaches
     * @param angle Rotation angle around trunk (radians)
     * @param armLength Length of the arm
     * @return Mesh for the arm
     */
    Mesh generateArm(float attachHeight, float angle, float armLength) const;

    Config m_config;
    float m_growthFactor = 1.0f;    // Multiplier for growth effects
    bool m_isBurning = false;        // Fire particle trigger
};