#include "MeshGenerator.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

Mesh MeshGenerator::createSphere(float radius, uint32_t segments, uint32_t rings, 
                                  const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (uint32_t ring = 0; ring <= rings; ++ring) {
        float phi = glm::pi<float>() * static_cast<float>(ring) / static_cast<float>(rings);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (uint32_t segment = 0; segment <= segments; ++segment) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(segment) / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            // Position on unit sphere, then scale by radius
            glm::vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            glm::vec3 position = normal * radius;

            // UV coordinates
            glm::vec2 texCoord(
                static_cast<float>(segment) / static_cast<float>(segments),
                static_cast<float>(ring) / static_cast<float>(rings)
            );

            vertices.push_back({position, normal, texCoord, color});
        }
    }

    // Generate indices
    for (uint32_t ring = 0; ring < rings; ++ring) {
        for (uint32_t segment = 0; segment < segments; ++segment) {
            uint32_t current = ring * (segments + 1) + segment;
            uint32_t next = current + segments + 1;

            // Two triangles per quad
            // Triangle 1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Triangle 2
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return Mesh(std::move(vertices), std::move(indices));
}

Mesh MeshGenerator::createPlane(float width, float depth, 
                                 uint32_t subdivisionsX, uint32_t subdivisionsZ,
                                 const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;

    // Generate vertices
    for (uint32_t z = 0; z <= subdivisionsZ; ++z) {
        for (uint32_t x = 0; x <= subdivisionsX; ++x) {
            float xPos = -halfWidth + (width * static_cast<float>(x) / static_cast<float>(subdivisionsX));
            float zPos = -halfDepth + (depth * static_cast<float>(z) / static_cast<float>(subdivisionsZ));

            glm::vec3 position(xPos, 0.0f, zPos);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);  // Pointing up
            glm::vec2 texCoord(
                static_cast<float>(x) / static_cast<float>(subdivisionsX),
                static_cast<float>(z) / static_cast<float>(subdivisionsZ)
            );

            vertices.push_back({position, normal, texCoord, color});
        }
    }

    // Generate indices
    for (uint32_t z = 0; z < subdivisionsZ; ++z) {
        for (uint32_t x = 0; x < subdivisionsX; ++x) {
            uint32_t topLeft = z * (subdivisionsX + 1) + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = topLeft + (subdivisionsX + 1);
            uint32_t bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return Mesh(std::move(vertices), std::move(indices));
}

Mesh MeshGenerator::createCylinder(float radius, float height, uint32_t segments,
                                    const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfHeight = height * 0.5f;

    // Generate side vertices (two rings)
    for (uint32_t i = 0; i <= 1; ++i) {
        float y = (i == 0) ? -halfHeight : halfHeight;
        float v = static_cast<float>(i);

        for (uint32_t seg = 0; seg <= segments; ++seg) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            glm::vec3 position(radius * cosTheta, y, radius * sinTheta);
            glm::vec3 normal(cosTheta, 0.0f, sinTheta);  // Outward facing
            glm::vec2 texCoord(static_cast<float>(seg) / static_cast<float>(segments), v);

            vertices.push_back({position, normal, texCoord, color});
        }
    }

    // Generate side indices
    for (uint32_t seg = 0; seg < segments; ++seg) {
        uint32_t bottom = seg;
        uint32_t top = seg + segments + 1;

        indices.push_back(bottom);
        indices.push_back(bottom + 1);
        indices.push_back(top);

        indices.push_back(top);
        indices.push_back(bottom + 1);
        indices.push_back(top + 1);
    }

    // Add top cap
    uint32_t topCenterIndex = static_cast<uint32_t>(vertices.size());
    vertices.push_back({{0.0f, halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}, color});

    for (uint32_t seg = 0; seg <= segments; ++seg) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        glm::vec3 position(radius * cosTheta, halfHeight, radius * sinTheta);
        glm::vec2 texCoord(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);

        vertices.push_back({position, {0.0f, 1.0f, 0.0f}, texCoord, color});
    }

    for (uint32_t seg = 0; seg < segments; ++seg) {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + 1 + seg + 1);
        indices.push_back(topCenterIndex + 1 + seg);
    }

    // Add bottom cap
    uint32_t bottomCenterIndex = static_cast<uint32_t>(vertices.size());
    vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}, color});

    for (uint32_t seg = 0; seg <= segments; ++seg) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        glm::vec3 position(radius * cosTheta, -halfHeight, radius * sinTheta);
        glm::vec2 texCoord(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);

        vertices.push_back({position, {0.0f, -1.0f, 0.0f}, texCoord, color});
    }

    for (uint32_t seg = 0; seg < segments; ++seg) {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + 1 + seg);
        indices.push_back(bottomCenterIndex + 1 + seg + 1);
    }

    return Mesh(std::move(vertices), std::move(indices));
}

Mesh MeshGenerator::createCone(float baseRadius, float topRadius, float height, 
                                uint32_t segments, const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfHeight = height * 0.5f;

    // Calculate normal slope for cone surface
    float slope = (baseRadius - topRadius) / height;
    float normalY = slope / std::sqrt(1.0f + slope * slope);
    float normalXZ = 1.0f / std::sqrt(1.0f + slope * slope);

    // Generate side vertices
    for (uint32_t i = 0; i <= 1; ++i) {
        float y = (i == 0) ? -halfHeight : halfHeight;
        float radius = (i == 0) ? baseRadius : topRadius;
        float v = static_cast<float>(i);

        for (uint32_t seg = 0; seg <= segments; ++seg) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            glm::vec3 position(radius * cosTheta, y, radius * sinTheta);
            glm::vec3 normal = glm::normalize(glm::vec3(normalXZ * cosTheta, normalY, normalXZ * sinTheta));
            glm::vec2 texCoord(static_cast<float>(seg) / static_cast<float>(segments), v);

            vertices.push_back({position, normal, texCoord, color});
        }
    }

    // Generate side indices
    for (uint32_t seg = 0; seg < segments; ++seg) {
        uint32_t bottom = seg;
        uint32_t top = seg + segments + 1;

        indices.push_back(bottom);
        indices.push_back(bottom + 1);
        indices.push_back(top);

        indices.push_back(top);
        indices.push_back(bottom + 1);
        indices.push_back(top + 1);
    }

    // Add bottom cap if baseRadius > 0
    if (baseRadius > 0.001f) {
        uint32_t centerIndex = static_cast<uint32_t>(vertices.size());
        vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}, color});

        for (uint32_t seg = 0; seg <= segments; ++seg) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            glm::vec3 position(baseRadius * cosTheta, -halfHeight, baseRadius * sinTheta);
            glm::vec2 texCoord(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);

            vertices.push_back({position, {0.0f, -1.0f, 0.0f}, texCoord, color});
        }

        for (uint32_t seg = 0; seg < segments; ++seg) {
            indices.push_back(centerIndex);
            indices.push_back(centerIndex + 1 + seg);
            indices.push_back(centerIndex + 1 + seg + 1);
        }
    }

    return Mesh(std::move(vertices), std::move(indices));
}