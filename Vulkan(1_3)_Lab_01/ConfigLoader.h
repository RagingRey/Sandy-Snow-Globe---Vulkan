#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Cactus.h"

/**
 * @brief Configuration file loader for scene setup
 * 
 * Role: Parse and provide scene configuration from external files
 * Responsibilities:
 * - Load cactus positions and properties
 * - Load OBJ model placements (animals, objects)
 * - Load day/night cycle parameters
 * - Load seasonal settings
 * - Provide defaults when config is missing
 * 
 * File Format: Simple key-value text format
 */
class ConfigLoader final{
public:
    /**
     * @brief Scene configuration data
     */
    struct SceneConfig {
        // Cactus placements
        std::vector<Cactus::Config> cacti;
        
        // Generic objects (animals, rocks, etc.) - load from OBJ files
        struct ObjectPlacement {
            std::string modelPath;      // Path to .obj file
            glm::vec3 position{0.0f};
            glm::vec3 scale{1.0f};
            float rotation = 0.0f;      // Y-axis rotation in degrees
        };
        std::vector<ObjectPlacement> objects;
        
        // Day/Night cycle
        glm::vec3 initialSunDirection{1.0f, 1.0f, 0.0f};
        float dayLengthSeconds = 60.0f;      // Real seconds per game day
        float nightLengthSeconds = 30.0f;    // Real seconds per game night
        
        // Seasons
        float seasonLengthDays = 5.0f;       // Game days per season
        bool enableRain = true;
        bool enableSnow = true;
        
        // Particle effects
        bool sandParticlesEnabled = true;
        float fireChance = 0.1f;             // Chance of spontaneous fire
    };

    /**
     * @brief Load configuration from file
     * @param filename Path to config file
     * @return Loaded configuration (defaults used for missing values)
     */
    static SceneConfig load(const std::string& filename);

    /**
     * @brief Get default configuration
     * @return Default scene setup
     */
    static SceneConfig getDefaults();

private:
    static void parseLine(const std::string& line, SceneConfig& config);
    static glm::vec3 parseVec3(const std::string& str);
    static float parseFloat(const std::string& str);
    static int parseInt(const std::string& str);
};