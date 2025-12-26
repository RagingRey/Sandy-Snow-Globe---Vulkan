#pragma once

#include "Vertex.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

/**
 * @brief Represents a 3D mesh with vertices and indices
 * 
 * Role: Data container for geometry loaded from OBJ files
 * Responsibilities:
 * - Store vertex and index data
 * - Provide bounding box information
 * - Support transformation
 * 
 * Design Notes:
 * - Separation from loader allows reuse with different file formats
 * - Bounding box useful for culling and collision
 */
class Mesh {
public:
    Mesh() = default;
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    // Accessors
    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getIndexCount() const { return m_indices.size(); }

    // Bounding box
    glm::vec3 getMinBounds() const { return m_minBounds; }
    glm::vec3 getMaxBounds() const { return m_maxBounds; }
    glm::vec3 getCenter() const { return (m_minBounds + m_maxBounds) * 0.5f; }

    // Modification
    void setVertices(std::vector<Vertex> vertices);
    void setIndices(std::vector<uint32_t> indices);
    void recalculateNormals();
    void recalculateBounds();

    // Utility
    bool isEmpty() const { return m_vertices.empty(); }

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    glm::vec3 m_minBounds{ 0.0f };
    glm::vec3 m_maxBounds{ 0.0f };
};