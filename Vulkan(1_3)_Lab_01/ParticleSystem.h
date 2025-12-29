#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <random>
#include <functional>
#include "Particle.h"

// ============================================================
// PARTICLE SYSTEM
// ============================================================
// Manages emission, physics simulation, and vertex generation
// for GPU rendering. Supports multiple effect types.
//
// Key concepts for defense:
// - Object pooling: Pre-allocated particles, recycled when dead
// - Billboard quads: 4 vertices per particle, always face camera
// - Procedural emission: Random within configurable bounds
// ============================================================

class ParticleSystem {
public:
    // Effect presets
    enum class EffectType {
        Fire,
        Smoke,
        Sand,
        Snow,
        Sparks
    };
    
    // Configuration for particle behavior
    struct EmitterConfig {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 positionVariance = glm::vec3(1.0f);   // Random offset range
        
        glm::vec3 velocity = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 velocityVariance = glm::vec3(1.0f);
        
        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        
        float startSize = 1.0f;
        float endSize = 0.1f;
        
        float minLife = 1.0f;
        float maxLife = 3.0f;
        
        float emissionRate = 50.0f;  // Particles per second
        int maxParticles = 1000;
        
        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        float drag = 0.1f;           // Velocity dampening
        
        bool looping = true;
        float duration = 0.0f;       // 0 = infinite (if looping)
    };

private:
    std::vector<Particle> particles;
    std::vector<ParticleVertex> vertices;  // 4 per particle (quad)
    std::vector<uint32_t> indices;         // 6 per particle (2 triangles)
    
    EmitterConfig config;
    bool active = false;
    float emissionAccumulator = 0.0f;
    float systemTime = 0.0f;
    
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist{0.0f, 1.0f};

public:
    ParticleSystem() : rng(std::random_device{}()) {}
    
    // Initialize with effect preset
    void init(EffectType type, const glm::vec3& position) {
        config = getPreset(type);
        config.position = position;
        particles.resize(config.maxParticles);
        
        // Pre-allocate vertex/index buffers
        vertices.reserve(config.maxParticles * 4);
        indices.reserve(config.maxParticles * 6);
        
        // Initialize all particles as dead
        for (auto& p : particles) {
            p.life = 0.0f;
        }
        
        active = true;
        systemTime = 0.0f;
    }
    
    // Initialize with custom config
    void init(const EmitterConfig& cfg) {
        config = cfg;
        particles.resize(config.maxParticles);
        vertices.reserve(config.maxParticles * 4);
        indices.reserve(config.maxParticles * 6);
        
        for (auto& p : particles) {
            p.life = 0.0f;
        }
        
        active = true;
        systemTime = 0.0f;
    }
    
    void setPosition(const glm::vec3& pos) {
        config.position = pos;
    }
    
    void start() { active = true; }
    void stop() { active = false; }
    bool isActive() const { return active; }
    
    // Update particle simulation
    void update(float deltaTime) {
        if (!active && getAliveCount() == 0) return;
        
        systemTime += deltaTime;
        
        // Check duration for non-looping effects
        if (!config.looping && config.duration > 0.0f && systemTime > config.duration) {
            active = false;
        }
        
        // Emit new particles
        if (active) {
            emissionAccumulator += config.emissionRate * deltaTime;
            while (emissionAccumulator >= 1.0f) {
                emitParticle();
                emissionAccumulator -= 1.0f;
            }
        }
        
        // Update existing particles
        for (auto& p : particles) {
            if (!p.isAlive()) continue;
            
            // Physics
            p.velocity += config.gravity * deltaTime;
            p.velocity *= (1.0f - config.drag * deltaTime);
            p.position += p.velocity * deltaTime;
            
            // Life
            p.life -= deltaTime;
            
            // Interpolate color and size based on age
            float age = p.getAge();
            p.color = glm::mix(config.startColor, config.endColor, age);
            p.size = glm::mix(config.startSize, config.endSize, age);
        }
    }
    
    // Generate billboard vertices for rendering
    void generateVertices(const glm::vec3& cameraRight, const glm::vec3& cameraUp) {
        vertices.clear();
        indices.clear();
        
        uint32_t vertexIndex = 0;
        
        for (const auto& p : particles) {
            if (!p.isAlive()) continue;
            
            // Billboard corners (camera-facing quad)
            glm::vec3 right = cameraRight * p.size;
            glm::vec3 up = cameraUp * p.size;
            
            // 4 corners of quad
            ParticleVertex v0, v1, v2, v3;
            
            v0.position = p.position - right - up;  // Bottom-left
            v0.color = p.color;
            v0.texCoord = glm::vec2(0.0f, 0.0f);
            v0.size = p.size;
            
            v1.position = p.position + right - up;  // Bottom-right
            v1.color = p.color;
            v1.texCoord = glm::vec2(1.0f, 0.0f);
            v1.size = p.size;
            
            v2.position = p.position + right + up;  // Top-right
            v2.color = p.color;
            v2.texCoord = glm::vec2(1.0f, 1.0f);
            v2.size = p.size;
            
            v3.position = p.position - right + up;  // Top-left
            v3.color = p.color;
            v3.texCoord = glm::vec2(0.0f, 1.0f);
            v3.size = p.size;
            
            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            // Two triangles per quad
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 2);
            
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 2);
            indices.push_back(vertexIndex + 3);
            
            vertexIndex += 4;
        }
    }
    
    // Accessors for rendering
    const std::vector<ParticleVertex>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }
    size_t getVertexCount() const { return vertices.size(); }
    size_t getIndexCount() const { return indices.size(); }
    
    int getAliveCount() const {
        int count = 0;
        for (const auto& p : particles) {
            if (p.isAlive()) count++;
        }
        return count;
    }

private:
    void emitParticle() {
        // Find dead particle to recycle
        for (auto& p : particles) {
            if (p.isAlive()) continue;
            
            // Random position within variance
            p.position = config.position + glm::vec3(
                randomRange(-config.positionVariance.x, config.positionVariance.x),
                randomRange(-config.positionVariance.y, config.positionVariance.y),
                randomRange(-config.positionVariance.z, config.positionVariance.z)
            );
            
            // Random velocity
            p.velocity = config.velocity + glm::vec3(
                randomRange(-config.velocityVariance.x, config.velocityVariance.x),
                randomRange(-config.velocityVariance.y, config.velocityVariance.y),
                randomRange(-config.velocityVariance.z, config.velocityVariance.z)
            );
            
            p.life = randomRange(config.minLife, config.maxLife);
            p.maxLife = p.life;
            p.color = config.startColor;
            p.size = config.startSize;
            
            return;
        }
    }
    
    float randomRange(float min, float max) {
        return min + dist(rng) * (max - min);
    }
    
    // Effect presets
    static EmitterConfig getPreset(EffectType type) {
        EmitterConfig cfg;
        
        switch (type) {
            case EffectType::Fire:
                cfg.velocity = glm::vec3(0.0f, 8.0f, 0.0f);
                cfg.velocityVariance = glm::vec3(2.0f, 3.0f, 2.0f);
                cfg.positionVariance = glm::vec3(0.5f, 0.1f, 0.5f);
                cfg.startColor = glm::vec4(1.0f, 0.6f, 0.1f, 1.0f);   // Orange
                cfg.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);     // Red, fade out
                cfg.startSize = 1.5f;
                cfg.endSize = 0.2f;
                cfg.minLife = 0.5f;
                cfg.maxLife = 1.5f;
                cfg.gravity = glm::vec3(0.0f, 2.0f, 0.0f);  // Fire rises
                cfg.drag = 0.5f;
                cfg.emissionRate = 80.0f;
                cfg.maxParticles = 500;
                break;
                
            case EffectType::Smoke:
                cfg.velocity = glm::vec3(0.0f, 3.0f, 0.0f);
                cfg.velocityVariance = glm::vec3(1.0f, 1.0f, 1.0f);
                cfg.positionVariance = glm::vec3(0.3f, 0.0f, 0.3f);
                cfg.startColor = glm::vec4(0.3f, 0.3f, 0.3f, 0.8f);
                cfg.endColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
                cfg.startSize = 0.5f;
                cfg.endSize = 3.0f;
                cfg.minLife = 2.0f;
                cfg.maxLife = 4.0f;
                cfg.gravity = glm::vec3(0.0f, 0.5f, 0.0f);
                cfg.drag = 0.2f;
                cfg.emissionRate = 20.0f;
                cfg.maxParticles = 200;
                break;
                
            case EffectType::Sand:
                cfg.velocity = glm::vec3(15.0f, 2.0f, 0.0f);  // Wind direction
                cfg.velocityVariance = glm::vec3(5.0f, 2.0f, 3.0f);
                cfg.positionVariance = glm::vec3(50.0f, 0.5f, 50.0f);
                cfg.startColor = glm::vec4(0.76f, 0.70f, 0.50f, 0.6f);
                cfg.endColor = glm::vec4(0.76f, 0.70f, 0.50f, 0.0f);
                cfg.startSize = 0.3f;
                cfg.endSize = 0.1f;
                cfg.minLife = 2.0f;
                cfg.maxLife = 5.0f;
                cfg.gravity = glm::vec3(0.0f, -2.0f, 0.0f);
                cfg.drag = 0.1f;
                cfg.emissionRate = 100.0f;
                cfg.maxParticles = 1000;
                break;
                
            case EffectType::Snow:
                cfg.velocity = glm::vec3(0.0f, -3.0f, 0.0f);
                cfg.velocityVariance = glm::vec3(2.0f, 1.0f, 2.0f);
                cfg.positionVariance = glm::vec3(80.0f, 50.0f, 80.0f);
                cfg.startColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.9f);
                cfg.endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
                cfg.startSize = 0.3f;
                cfg.endSize = 0.2f;
                cfg.minLife = 5.0f;
                cfg.maxLife = 10.0f;
                cfg.gravity = glm::vec3(0.0f, -1.0f, 0.0f);
                cfg.drag = 0.3f;
                cfg.emissionRate = 50.0f;
                cfg.maxParticles = 800;
                break;
                
            case EffectType::Sparks:
                cfg.velocity = glm::vec3(0.0f, 15.0f, 0.0f);
                cfg.velocityVariance = glm::vec3(8.0f, 5.0f, 8.0f);
                cfg.positionVariance = glm::vec3(0.2f, 0.0f, 0.2f);
                cfg.startColor = glm::vec4(1.0f, 0.9f, 0.3f, 1.0f);
                cfg.endColor = glm::vec4(1.0f, 0.3f, 0.0f, 0.0f);
                cfg.startSize = 0.2f;
                cfg.endSize = 0.05f;
                cfg.minLife = 0.3f;
                cfg.maxLife = 1.0f;
                cfg.gravity = glm::vec3(0.0f, -15.0f, 0.0f);
                cfg.drag = 0.05f;
                cfg.emissionRate = 150.0f;
                cfg.maxParticles = 300;
                break;
        }
        
        return cfg;
    }
};