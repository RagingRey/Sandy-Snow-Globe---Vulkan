#include "ConfigLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ConfigLoader::SceneConfig ConfigLoader::load(const std::string& filename) {
    SceneConfig config = getDefaults();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file not found: " << filename 
                  << ", using defaults\n";
        return config;
    }
    
    // Clear default cacti if loading from file
    config.cacti.clear();
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        try {
            parseLine(line, config);
        } catch (const std::exception& e) {
            std::cerr << "Config error at line " << lineNum 
                      << ": " << e.what() << "\n";
        }
    }
    
    std::cout << "Loaded config: " << config.cacti.size() << " cacti\n";
    return config;
}

ConfigLoader::SceneConfig ConfigLoader::getDefaults() {
    SceneConfig config;
    
    // Default cacti (same as current hardcoded values)
    Cactus::Config cactus1;
    cactus1.position = glm::vec3(20.0f, 0.0f, 15.0f);
    cactus1.height = 12.0f;
    cactus1.trunkRadius = 0.8f;
    cactus1.numArms = 3;
    cactus1.color = glm::vec3(0.15f, 0.5f, 0.15f);
    config.cacti.push_back(cactus1);
    
    Cactus::Config cactus2;
    cactus2.position = glm::vec3(-30.0f, 0.0f, -20.0f);
    cactus2.height = 8.0f;
    cactus2.trunkRadius = 0.6f;
    cactus2.numArms = 2;
    cactus2.color = glm::vec3(0.2f, 0.55f, 0.2f);
    config.cacti.push_back(cactus2);
    
    Cactus::Config cactus3;
    cactus3.position = glm::vec3(40.0f, 0.0f, 40.0f);
    cactus3.height = 4.0f;
    cactus3.trunkRadius = 0.4f;
    cactus3.numArms = 1;
    cactus3.color = glm::vec3(0.25f, 0.6f, 0.25f);
    config.cacti.push_back(cactus3);
    
    return config;
}

void ConfigLoader::parseLine(const std::string& line, SceneConfig& config) {
    std::istringstream iss(line);
    std::string keyword;
    iss >> keyword;
    
    // Convert to lowercase for case-insensitive matching
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    
    if (keyword == "cactus") {
        // Format: cactus x y z height radius arms [r g b]
        Cactus::Config cactus;
        iss >> cactus.position.x >> cactus.position.y >> cactus.position.z
            >> cactus.height >> cactus.trunkRadius >> cactus.numArms;
        
        // Optional color
        float r, g, b;
        if (iss >> r >> g >> b) {
            cactus.color = glm::vec3(r, g, b);
        } else {
            cactus.color = glm::vec3(0.2f, 0.55f, 0.2f);  // Default green
        }
        
        config.cacti.push_back(cactus);
    }
    else if (keyword == "sun_direction") {
        iss >> config.initialSunDirection.x 
            >> config.initialSunDirection.y 
            >> config.initialSunDirection.z;
    }
    else if (keyword == "day_length") {
        iss >> config.dayLengthSeconds;
    }
    else if (keyword == "night_length") {
        iss >> config.nightLengthSeconds;
    }
    else if (keyword == "season_length") {
        iss >> config.seasonLengthDays;
    }
    else if (keyword == "enable_rain") {
        std::string val;
        iss >> val;
        config.enableRain = (val == "true" || val == "1");
    }
    else if (keyword == "enable_snow") {
        std::string val;
        iss >> val;
        config.enableSnow = (val == "true" || val == "1");
    }
    else if (keyword == "sand_particles") {
        std::string val;
        iss >> val;
        config.sandParticlesEnabled = (val == "true" || val == "1");
    }
    else if (keyword == "fire_chance") {
        iss >> config.fireChance;
    }
    else if (keyword == "object" || keyword == "animal") {
        // Format: object models/armadillo.obj x y z [scale] [rotation]
        SceneConfig::ObjectPlacement obj;
        iss >> obj.modelPath 
            >> obj.position.x >> obj.position.y >> obj.position.z;
        
        float scale = 1.0f;
        if (iss >> scale) {
            obj.scale = glm::vec3(scale);
            float rotation = 0.0f;
            if (iss >> rotation) {
                obj.rotation = rotation;
            }
        }
        
        config.objects.push_back(obj);
    }
    else {
        std::cerr << "Unknown config keyword: " << keyword << "\n";
    }
}

glm::vec3 ConfigLoader::parseVec3(const std::string& str) {
    glm::vec3 v;
    std::istringstream iss(str);
    iss >> v.x >> v.y >> v.z;
    return v;
}

float ConfigLoader::parseFloat(const std::string& str) {
    return std::stof(str);
}

int ConfigLoader::parseInt(const std::string& str) {
    return std::stoi(str);
}