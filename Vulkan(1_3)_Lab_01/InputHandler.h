#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <unordered_map>

/**
 * @brief Centralized input handling for keyboard controls
 * 
 * Design Notes for Defense:
 * - Decouples input from application logic
 * - Supports continuous (held) and single-press keys
 * - Easy to extend for new controls
 */
class InputHandler {
public:
    // Callback types
    using KeyCallback = std::function<void()>;

    void setupCallbacks(GLFWwindow* window);
    void processInput(GLFWwindow* window, float deltaTime);

    // Register callbacks for specific actions
    void onExit(KeyCallback callback) { m_onExit = callback; }
    void onReset(KeyCallback callback) { m_onReset = callback; }
    void onCameraSwitch(int cameraIndex, KeyCallback callback);
    void onParticleEffect(KeyCallback callback) { m_onParticleEffect = callback; }
    void onTimeDecrease(KeyCallback callback) { m_onTimeDecrease = callback; }
    void onTimeIncrease(KeyCallback callback) { m_onTimeIncrease = callback; }

    // Camera movement callbacks
    void onRotateLeft(KeyCallback callback) { m_onRotateLeft = callback; }
    void onRotateRight(KeyCallback callback) { m_onRotateRight = callback; }
    void onRotateUp(KeyCallback callback) { m_onRotateUp = callback; }
    void onRotateDown(KeyCallback callback) { m_onRotateDown = callback; }
    void onPanLeft(KeyCallback callback) { m_onPanLeft = callback; }
    void onPanRight(KeyCallback callback) { m_onPanRight = callback; }
    void onPanForward(KeyCallback callback) { m_onPanForward = callback; }
    void onPanBackward(KeyCallback callback) { m_onPanBackward = callback; }
    void onPanUp(KeyCallback callback) { m_onPanUp = callback; }
    void onPanDown(KeyCallback callback) { m_onPanDown = callback; }

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    // Single-press callbacks
    KeyCallback m_onExit;
    KeyCallback m_onReset;
    KeyCallback m_onParticleEffect;
    KeyCallback m_onTimeDecrease;
    KeyCallback m_onTimeIncrease;
    std::unordered_map<int, KeyCallback> m_cameraSwitchCallbacks;

    // Continuous (held) callbacks
    KeyCallback m_onRotateLeft;
    KeyCallback m_onRotateRight;
    KeyCallback m_onRotateUp;
    KeyCallback m_onRotateDown;
    KeyCallback m_onPanLeft;
    KeyCallback m_onPanRight;
    KeyCallback m_onPanForward;
    KeyCallback m_onPanBackward;
    KeyCallback m_onPanUp;
    KeyCallback m_onPanDown;
};