#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

// ============================================================
// DAY/NIGHT CYCLE SYSTEM
// ============================================================
// Simulates sun and moon movement with color transitions.
// Full cycle duration is configurable (default 60 seconds).
// 
// Key concepts for defense:
// - Uses sinusoidal functions to simulate celestial body arcs
// - Light color interpolates between warm (day) and cool (night)
// - Ambient strength varies to simulate atmospheric scattering
// ============================================================

class DayNightCycle {
public:
    // Cycle phases for seasonal effects
    enum class Phase {
        Dawn,       // 0.0 - 0.25
        Day,        // 0.25 - 0.5
        Dusk,       // 0.5 - 0.75
        Night       // 0.75 - 1.0
    };

    struct LightState {
        glm::vec3 position;         // Light source position
        glm::vec3 color;            // Light color (RGB)
        float intensity;            // Light brightness
        float ambientStrength;      // Ambient light level
        glm::vec3 skyColor;         // Background sky color
        bool isSunActive;           // true = sun, false = moon
    };

private:
    // Configuration
    float cycleDuration = 60.0f;    // Full day cycle in seconds
    float orbitRadius = 200.0f;     // Distance of sun/moon from center
    
    // Current state
    float currentTime = 0.0f;       // Accumulated time
    float cycleProgress = 0.0f;     // 0.0 to 1.0 (full day)
    
    // Colors
    const glm::vec3 sunColorNoon    = glm::vec3(1.0f, 0.95f, 0.9f);    // Warm white
    const glm::vec3 sunColorDawn    = glm::vec3(1.0f, 0.6f, 0.3f);     // Orange
    const glm::vec3 sunColorDusk    = glm::vec3(1.0f, 0.4f, 0.2f);     // Deep orange
    const glm::vec3 moonColor       = glm::vec3(0.6f, 0.7f, 0.9f);     // Cool blue-white
    
    // Sky colors
    const glm::vec3 skyDay          = glm::vec3(0.4f, 0.6f, 0.9f);     // Blue sky
    const glm::vec3 skyDawn         = glm::vec3(0.9f, 0.5f, 0.3f);     // Orange-pink
    const glm::vec3 skyDusk         = glm::vec3(0.6f, 0.3f, 0.4f);     // Purple-red
    const glm::vec3 skyNight        = glm::vec3(0.02f, 0.02f, 0.05f);  // Dark blue

public:
    DayNightCycle() = default;
    
    // Set cycle duration (seconds for full day)
    void setCycleDuration(float duration) {
        cycleDuration = duration;
    }
    
    void setOrbitRadius(float radius) {
        orbitRadius = radius;
    }
    
    // Update with delta time and time scale
    void update(float deltaTime, float timeScale = 1.0f) {
        currentTime += deltaTime * timeScale;
        cycleProgress = fmod(currentTime / cycleDuration, 1.0f);
    }
    
    // Get current phase
    Phase getPhase() const {
        if (cycleProgress < 0.25f) return Phase::Dawn;
        if (cycleProgress < 0.5f) return Phase::Day;
        if (cycleProgress < 0.75f) return Phase::Dusk;
        return Phase::Night;
    }
    
    // Get current cycle progress (0.0 - 1.0)
    float getProgress() const {
        return cycleProgress;
    }
    
    // Calculate current light state
    LightState getLightState() const {
        LightState state;
        
        // Convert cycle progress to angle (0 = dawn horizon, 0.5 = dusk horizon)
        // Sun rises from East, sets in West
        float sunAngle = cycleProgress * glm::two_pi<float>();
        
        // Sun/Moon position calculation
        // Y component = height (sin gives arc motion)
        // X component = horizontal position (cos gives east-west motion)
        float sunHeight = sin(sunAngle);            // -1 to 1
        float sunHorizontal = cos(sunAngle);        // East-West position
        
        // Determine if sun or moon is active
        state.isSunActive = (sunHeight > -0.1f);    // Sun visible when above horizon
        
        if (state.isSunActive) {
            // Sun position - arc across sky
            state.position = glm::vec3(
                sunHorizontal * orbitRadius,        // X: East to West
                std::max(0.0f, sunHeight) * orbitRadius + 50.0f, // Y: Height above horizon
                -0.3f * orbitRadius                 // Z: Slightly offset
            );
            
            // Sun color based on height
            float heightFactor = std::max(0.0f, sunHeight);
            if (heightFactor < 0.3f) {
                // Near horizon: orange tint
                float t = heightFactor / 0.3f;
                state.color = glm::mix(sunColorDawn, sunColorNoon, t);
            } else {
                state.color = sunColorNoon;
            }
            
            // Intensity based on sun height
            state.intensity = 0.3f + 0.7f * heightFactor;
            state.ambientStrength = 0.1f + 0.15f * heightFactor;
            
        } else {
            // Moon position - opposite of sun
            state.position = glm::vec3(
                -sunHorizontal * orbitRadius,
                std::max(0.0f, -sunHeight) * orbitRadius * 0.8f + 30.0f,
                0.3f * orbitRadius
            );
            state.color = moonColor;
            state.intensity = 0.15f + 0.1f * std::max(0.0f, -sunHeight);
            state.ambientStrength = 0.05f;
        }
        
        // Sky color interpolation
        state.skyColor = calculateSkyColor();
        
        return state;
    }
    
    // Reset to specific time of day (0.0 = dawn, 0.25 = noon, 0.5 = dusk, 0.75 = midnight)
    void setTimeOfDay(float progress) {
        cycleProgress = fmod(progress, 1.0f);
        currentTime = cycleProgress * cycleDuration;
    }
    
    void reset() {
        currentTime = 0.0f;
        cycleProgress = 0.0f;
    }

private:
    glm::vec3 calculateSkyColor() const {
        // Smooth transitions between sky colors
        if (cycleProgress < 0.15f) {
            // Night to Dawn
            float t = cycleProgress / 0.15f;
            return glm::mix(skyNight, skyDawn, t);
        } 
        else if (cycleProgress < 0.3f) {
            // Dawn to Day
            float t = (cycleProgress - 0.15f) / 0.15f;
            return glm::mix(skyDawn, skyDay, t);
        } 
        else if (cycleProgress < 0.5f) {
            // Full Day
            return skyDay;
        } 
        else if (cycleProgress < 0.65f) {
            // Day to Dusk
            float t = (cycleProgress - 0.5f) / 0.15f;
            return glm::mix(skyDay, skyDusk, t);
        } 
        else if (cycleProgress < 0.8f) {
            // Dusk to Night
            float t = (cycleProgress - 0.65f) / 0.15f;
            return glm::mix(skyDusk, skyNight, t);
        }
        // Full Night
        return skyNight;
    }
};