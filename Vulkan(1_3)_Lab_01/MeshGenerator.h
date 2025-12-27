#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

/**
 * @brief Procedural mesh generation utility
 * 
 * Role: Generate primitive meshes mathematically
 * Responsibilities:
 * - Create spheres (for globe)
 * - Create planes (for ground)
 * - Create cylinders/cones (for cacti)
 * 
 * Design Notes:
 * - Static utility class (stateless)
 * - All meshes have proper normals and UVs
 * - Configurable resolution for LOD support
 */
class MeshGenerator {
public:
    MeshGenerator() = delete;  // Prevent instantiation

    /**
     * @brief Generate a UV sphere mesh
     * @param radius Sphere radius
     * @param segments Horizontal divisions (longitude)
     * @param rings Vertical divisions (latitude)
     * @param color Vertex color
     * @return Generated sphere mesh
     * 
     * Used for: Globe (200 unit diameter = 100 radius)
     */
    static Mesh createSphere(float radius, uint32_t segments, uint32_t rings, 
                             const glm::vec3& color = glm::vec3(1.0f));

    /**
     * @brief Generate a flat plane mesh
     * @param width Width along X axis
     * @param depth Depth along Z axis
     * @param subdivisionsX Subdivisions along X
     * @param subdivisionsZ Subdivisions along Z
     * @param color Vertex color
     * @return Generated plane mesh
     * 
     * Used for: Desert ground plane at globe equator
     */
    static Mesh createPlane(float width, float depth, 
                            uint32_t subdivisionsX, uint32_t subdivisionsZ,
                            const glm::vec3& color = glm::vec3(1.0f));

    /**
     * @brief Generate a cylinder mesh
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param segments Number of segments around circumference
     * @param color Vertex color
     * @return Generated cylinder mesh
     * 
     * Used for: Cactus trunks
     */
    static Mesh createCylinder(float radius, float height, uint32_t segments,
                               const glm::vec3& color = glm::vec3(1.0f));

    /**
     * @brief Generate a cone mesh
     * @param baseRadius Radius at base
     * @param topRadius Radius at top (0 for pointed cone)
     * @param height Cone height
     * @param segments Number of segments around circumference
     * @param color Vertex color
     * @return Generated cone mesh
     * 
     * Used for: Cactus arms/leaves
     */
    static Mesh createCone(float baseRadius, float topRadius, float height, 
                           uint32_t segments, const glm::vec3& color = glm::vec3(1.0f));
};