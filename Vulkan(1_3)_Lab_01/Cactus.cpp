#include "Cactus.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Cactus::Cactus(const Config& config) 
    : m_config(config) {
}

Mesh Cactus::generateMesh() const {
    std::vector<Vertex> allVertices;
    std::vector<uint32_t> allIndices;

    float actualHeight = m_config.height * m_growthFactor;
    float actualRadius = m_config.trunkRadius * m_growthFactor;

    // Generate main trunk (capped cylinder)
    Mesh trunk = MeshGenerator::createCylinder(
        actualRadius,
        actualHeight,
        m_config.segments,
        m_config.color
    );

    // Transform trunk to world position (centered at base)
    const auto& trunkVerts = trunk.getVertices();
    const auto& trunkInds = trunk.getIndices();
    
    for (const auto& v : trunkVerts) {
        Vertex transformed = v;
        // Move from center to base, then to world position
        transformed.position.y += actualHeight * 0.5f;
        transformed.position += m_config.position;
        allVertices.push_back(transformed);
    }
    allIndices.insert(allIndices.end(), trunkInds.begin(), trunkInds.end());

    // Generate arms (Saguaro style)
    for (int i = 0; i < m_config.numArms; ++i) {
        float angle = (static_cast<float>(i) / m_config.numArms) * glm::two_pi<float>();
        angle += glm::pi<float>() * 0.25f; // Offset for visual interest
        
        float attachHeight = actualHeight * m_config.armHeight;
        float armLength = actualHeight * 0.4f;
        
        Mesh arm = generateArm(attachHeight, angle, armLength);
        
        uint32_t indexOffset = static_cast<uint32_t>(allVertices.size());
        const auto& armVerts = arm.getVertices();
        const auto& armInds = arm.getIndices();
        
        allVertices.insert(allVertices.end(), armVerts.begin(), armVerts.end());
        for (uint32_t idx : armInds) {
            allIndices.push_back(idx + indexOffset);
        }
    }

    return Mesh(std::move(allVertices), std::move(allIndices));
}

Mesh Cactus::generateArm(float attachHeight, float angle, float armLength) const {
    std::vector<Vertex> armVertices;
    std::vector<uint32_t> armIndices;

    float armRadius = m_config.trunkRadius * 0.6f * m_growthFactor;
    float elbowLength = armLength * 0.5f;

    // Horizontal section (elbow)
    Mesh horizontal = MeshGenerator::createCylinder(
        armRadius, elbowLength, m_config.segments, m_config.color
    );

    // Vertical section (upper arm)
    Mesh vertical = MeshGenerator::createCylinder(
        armRadius, armLength * 0.6f, m_config.segments, m_config.color
    );

    // Position horizontal section
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, m_config.position);
    transform = glm::translate(transform, glm::vec3(0.0f, attachHeight, 0.0f));
    transform = glm::rotate(transform, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(0.0f, elbowLength * 0.5f + m_config.trunkRadius, 0.0f));

    const auto& hVerts = horizontal.getVertices();
    const auto& hInds = horizontal.getIndices();
    
    for (const auto& v : hVerts) {
        Vertex transformed = v;
        glm::vec4 pos = transform * glm::vec4(v.position, 1.0f);
        transformed.position = glm::vec3(pos);
        // Transform normal
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        transformed.normal = glm::normalize(normalMatrix * v.normal);
        armVertices.push_back(transformed);
    }
    armIndices.insert(armIndices.end(), hInds.begin(), hInds.end());

    // Position vertical section at end of horizontal
    glm::mat4 vertTransform = glm::mat4(1.0f);
    vertTransform = glm::translate(vertTransform, m_config.position);
    vertTransform = glm::translate(vertTransform, glm::vec3(0.0f, attachHeight, 0.0f));
    vertTransform = glm::rotate(vertTransform, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    vertTransform = glm::translate(vertTransform, glm::vec3(elbowLength + m_config.trunkRadius, armLength * 0.3f, 0.0f));

    uint32_t indexOffset = static_cast<uint32_t>(armVertices.size());
    const auto& vVerts = vertical.getVertices();
    const auto& vInds = vertical.getIndices();
    
    for (const auto& v : vVerts) {
        Vertex transformed = v;
        glm::vec4 pos = vertTransform * glm::vec4(v.position, 1.0f);
        transformed.position = glm::vec3(pos);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(vertTransform)));
        transformed.normal = glm::normalize(normalMatrix * v.normal);
        armVertices.push_back(transformed);
    }
    for (uint32_t idx : vInds) {
        armIndices.push_back(idx + indexOffset);
    }

    return Mesh(std::move(armVertices), std::move(armIndices));
}

void Cactus::grow(float amount) {
    m_growthFactor += amount;
    // Spec says Saguaros can grow to 20m
    m_growthFactor = glm::clamp(m_growthFactor, 0.1f, 4.0f);
}