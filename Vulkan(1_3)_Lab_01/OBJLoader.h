#pragma once

#include "Mesh.h"
#include <string>
#include <memory>

/**
 * @brief Wavefront OBJ file loader
 * 
 * Role: Parse OBJ files and create Mesh objects
 * Responsibilities:
 * - Read and parse OBJ file format
 * - Handle vertices, normals, texture coordinates, and faces
 * - Support basic OBJ features required for this project
 * 
 * Design Notes:
 * - Static utility class (no state needed)
 * - Returns Mesh by value for simplicity
 * - Handles missing normals by calculating them
 * 
 * OBJ Format Reference: http://www.fileformat.info/format/wavefrontobj/
 * Supported elements: v, vt, vn, f
 */
class OBJLoader {
public:
    // Prevent instantiation - utility class
    OBJLoader() = delete;

    /**
     * @brief Load a mesh from an OBJ file
     * @param filepath Path to the OBJ file
     * @return Loaded mesh, empty mesh if loading fails
     */
    static Mesh load(const std::string& filepath);

    /**
     * @brief Check if a file exists and is readable
     * @param filepath Path to check
     * @return true if file exists and is readable
     */
    static bool fileExists(const std::string& filepath);

private:
    struct FaceVertex {
        int positionIndex = -1;
        int texCoordIndex = -1;
        int normalIndex = -1;
    };

    static FaceVertex parseFaceVertex(const std::string& token);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
};