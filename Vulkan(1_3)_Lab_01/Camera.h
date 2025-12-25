#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Camera class for 3D scene navigation
 * 
 * Supports two movement modes:
 * - Rotation: Orbiting around a target point (pitch/yaw)
 * - Panning: Translation in world space
 * 
 * Design Notes for Defense:
 * - Uses spherical coordinates for intuitive rotation
 * - Stores initial state for reset functionality
 * - Decoupled from input handling for reusability
 */
class Camera {
public:
    enum class Type {
        Overview,    // C1: Fixed overview of the globe
        Navigation,  // C2: Free navigation camera
        CloseUp      // C3: Close-up on specific object
    };

    Camera() = default;
    
    /**
     * @brief Construct a camera with initial parameters
     * @param position Initial world position
     * @param target Point the camera looks at
     * @param up World up vector
     * @param type Camera type identifier
     */
    Camera(const glm::vec3& position, const glm::vec3& target, 
           const glm::vec3& up, Type type = Type::Navigation);

    // View matrix generation
    glm::mat4 getViewMatrix() const;

    // Rotation controls (in degrees)
    void rotateYaw(float angleDegrees);    // Left/Right rotation
    void rotatePitch(float angleDegrees);  // Up/Down rotation

    // Panning controls (world-space translation)
    void panHorizontal(float amount);  // Left/Right panning
    void panForward(float amount);     // Forward/Backward panning
    void panVertical(float amount);    // Up/Down panning

    // Reset to initial state
    void reset();

    // Getters
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getTarget() const { return m_target; }
    glm::vec3 getUp() const { return m_up; }
    Type getType() const { return m_type; }

    // Setters for configuration
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setRotationSpeed(float speed) { m_rotationSpeed = speed; }
    void setPanSpeed(float speed) { m_panSpeed = speed; }

private:
    void updateVectorsFromSpherical();
    void updateSphericalFromVectors();

    // Current state
    glm::vec3 m_position{ 0.0f, 0.0f, 3.0f };
    glm::vec3 m_target{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };

    // Spherical coordinates for rotation (relative to target)
    float m_yaw = -90.0f;      // Horizontal angle (degrees)
    float m_pitch = 0.0f;      // Vertical angle (degrees)
    float m_distance = 3.0f;   // Distance from target

    // Initial state for reset
    glm::vec3 m_initialPosition{ 0.0f, 0.0f, 3.0f };
    glm::vec3 m_initialTarget{ 0.0f, 0.0f, 0.0f };
    glm::vec3 m_initialUp{ 0.0f, 1.0f, 0.0f };
    float m_initialYaw = -90.0f;
    float m_initialPitch = 0.0f;

    // Movement parameters
    float m_rotationSpeed = 2.0f;  // Degrees per input
    float m_panSpeed = 0.5f;       // Units per input

    // Constraints
    static constexpr float MAX_PITCH = 89.0f;
    static constexpr float MIN_PITCH = -89.0f;

    Type m_type = Type::Navigation;
};