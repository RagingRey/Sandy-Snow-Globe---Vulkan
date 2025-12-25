#include "Camera.h"
#include <algorithm>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, 
               const glm::vec3& up, Type type)
    : m_position(position)
    , m_target(target)
    , m_up(up)
    , m_initialPosition(position)
    , m_initialTarget(target)
    , m_initialUp(up)
    , m_type(type)
{
    // Calculate initial spherical coordinates from position/target
    updateSphericalFromVectors();
    m_initialYaw = m_yaw;
    m_initialPitch = m_pitch;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

void Camera::rotateYaw(float angleDegrees) {
    m_yaw += angleDegrees * m_rotationSpeed;
    updateVectorsFromSpherical();
}

void Camera::rotatePitch(float angleDegrees) {
    m_pitch += angleDegrees * m_rotationSpeed;
    // Clamp pitch to prevent camera flipping
    m_pitch = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);
    updateVectorsFromSpherical();
}

void Camera::panHorizontal(float amount) {
    // Calculate right vector from current view direction
    glm::vec3 forward = glm::normalize(m_target - m_position);
    glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
    
    glm::vec3 offset = right * amount * m_panSpeed;
    m_position += offset;
    m_target += offset;
}

void Camera::panForward(float amount) {
    // Move along the view direction (projected onto horizontal plane)
    glm::vec3 forward = glm::normalize(m_target - m_position);
    // Keep movement horizontal by zeroing Y component
    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    
    glm::vec3 offset = horizontalForward * amount * m_panSpeed;
    m_position += offset;
    m_target += offset;
}

void Camera::panVertical(float amount) {
    glm::vec3 offset = glm::vec3(0.0f, amount * m_panSpeed, 0.0f);
    m_position += offset;
    m_target += offset;
}

void Camera::reset() {
    m_position = m_initialPosition;
    m_target = m_initialTarget;
    m_up = m_initialUp;
    m_yaw = m_initialYaw;
    m_pitch = m_initialPitch;
    updateSphericalFromVectors();
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    updateSphericalFromVectors();
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
    updateSphericalFromVectors();
}

void Camera::updateVectorsFromSpherical() {
    // Convert spherical to Cartesian coordinates
    // Position orbits around target at distance m_distance
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    glm::vec3 offset;
    offset.x = m_distance * cos(pitchRad) * cos(yawRad);
    offset.y = m_distance * sin(pitchRad);
    offset.z = m_distance * cos(pitchRad) * sin(yawRad);

    m_position = m_target + offset;
}

void Camera::updateSphericalFromVectors() {
    glm::vec3 offset = m_position - m_target;
    m_distance = glm::length(offset);
    
    if (m_distance > 0.001f) {
        // Calculate yaw (horizontal angle)
        m_yaw = glm::degrees(atan2(offset.z, offset.x));
        // Calculate pitch (vertical angle)
        m_pitch = glm::degrees(asin(offset.y / m_distance));
        m_pitch = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);
    }
}