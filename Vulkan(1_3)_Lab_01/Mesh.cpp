#include "Mesh.h"
#include <limits>
#include <cmath>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_vertices(std::move(vertices))
    , m_indices(std::move(indices))
{
    recalculateBounds();
}

void Mesh::setVertices(std::vector<Vertex> vertices) {
    m_vertices = std::move(vertices);
    recalculateBounds();
}

void Mesh::setIndices(std::vector<uint32_t> indices) {
    m_indices = std::move(indices);
}

void Mesh::recalculateBounds() {
    if (m_vertices.empty()) {
        m_minBounds = glm::vec3(0.0f);
        m_maxBounds = glm::vec3(0.0f);
        return;
    }

    m_minBounds = glm::vec3(std::numeric_limits<float>::max());
    m_maxBounds = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& vertex : m_vertices) {
        m_minBounds = glm::min(m_minBounds, vertex.position);
        m_maxBounds = glm::max(m_maxBounds, vertex.position);
    }
}

void Mesh::recalculateNormals() {
    // Reset all normals to zero
    for (auto& vertex : m_vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    // Calculate face normals and accumulate to vertices
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        uint32_t i0 = m_indices[i];
        uint32_t i1 = m_indices[i + 1];
        uint32_t i2 = m_indices[i + 2];

        glm::vec3 v0 = m_vertices[i0].position;
        glm::vec3 v1 = m_vertices[i1].position;
        glm::vec3 v2 = m_vertices[i2].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        m_vertices[i0].normal += faceNormal;
        m_vertices[i1].normal += faceNormal;
        m_vertices[i2].normal += faceNormal;
    }

    // Normalize all vertex normals
    for (auto& vertex : m_vertices) {
        float length = glm::length(vertex.normal);
        if (length > 0.0001f) {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }
}