#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

Mesh OBJLoader::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "OBJLoader: Failed to open file: " << filepath << std::endl;
        return Mesh();
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Map for vertex deduplication
    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Vertex position
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt") {
            // Texture coordinate
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            // Flip V coordinate for Vulkan (OBJ uses bottom-left origin)
            tex.y = 1.0f - tex.y;
            texCoords.push_back(tex);
        }
        else if (prefix == "vn") {
            // Vertex normal
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (prefix == "f") {
            // Face - can be triangle or polygon
            std::vector<FaceVertex> faceVertices;
            std::string token;
            
            while (iss >> token) {
                faceVertices.push_back(parseFaceVertex(token));
            }

            // Triangulate polygon (fan triangulation)
            for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                std::array<FaceVertex, 3> triangle = {
                    faceVertices[0],
                    faceVertices[i],
                    faceVertices[i + 1]
                };

                for (const auto& fv : triangle) {
                    Vertex vertex{};

                    // Position (required)
                    if (fv.positionIndex >= 0 && 
                        fv.positionIndex < static_cast<int>(positions.size())) {
                        vertex.position = positions[fv.positionIndex];
                    }

                    // Normal (optional)
                    if (fv.normalIndex >= 0 && 
                        fv.normalIndex < static_cast<int>(normals.size())) {
                        vertex.normal = normals[fv.normalIndex];
                    }

                    // Texture coordinate (optional)
                    if (fv.texCoordIndex >= 0 && 
                        fv.texCoordIndex < static_cast<int>(texCoords.size())) {
                        vertex.texCoord = texCoords[fv.texCoordIndex];
                    }

                    // Default color (white)
                    vertex.color = glm::vec3(1.0f);

                    // Deduplicate vertices
                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }
                    indices.push_back(uniqueVertices[vertex]);
                }
            }
        }
        // Ignore: mtllib, usemtl, o, g, s (not needed for basic loading)
    }

    file.close();

    Mesh mesh(std::move(vertices), std::move(indices));

    // Calculate normals if none were provided
    bool hasNormals = !normals.empty();
    if (!hasNormals) {
        mesh.recalculateNormals();
    }

    std::cout << "OBJLoader: Loaded " << filepath 
              << " (" << mesh.getVertexCount() << " vertices, "
              << mesh.getIndexCount() / 3 << " triangles)" << std::endl;

    return mesh;
}

bool OBJLoader::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

OBJLoader::FaceVertex OBJLoader::parseFaceVertex(const std::string& token) {
    FaceVertex fv;
    std::vector<std::string> parts = split(token, '/');

    // Format: v, v/vt, v/vt/vn, v//vn
    if (!parts.empty() && !parts[0].empty()) {
        fv.positionIndex = std::stoi(parts[0]) - 1;  // OBJ is 1-indexed
    }
    if (parts.size() > 1 && !parts[1].empty()) {
        fv.texCoordIndex = std::stoi(parts[1]) - 1;
    }
    if (parts.size() > 2 && !parts[2].empty()) {
        fv.normalIndex = std::stoi(parts[2]) - 1;
    }

    return fv;
}

std::vector<std::string> OBJLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string OBJLoader::trim(const std::string& str) {
    const char* whitespace = " \t\r\n";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}