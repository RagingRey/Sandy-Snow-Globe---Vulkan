#include "InputHandler.h"

void InputHandler::setupCallbacks(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void InputHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (!handler) return;

    // Handle single-press events
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                if (handler->m_onExit) handler->m_onExit();
                break;
            case GLFW_KEY_R:
                if (handler->m_onReset) handler->m_onReset();
                break;
            case GLFW_KEY_F1:
            case GLFW_KEY_F2:
            case GLFW_KEY_F3:
                if (handler->m_cameraSwitchCallbacks.count(key)) {
                    handler->m_cameraSwitchCallbacks[key]();
                }
                break;
            case GLFW_KEY_F4:
                if (handler->m_onParticleEffect) handler->m_onParticleEffect();
                break;
            case GLFW_KEY_T:
                if (mods & GLFW_MOD_SHIFT) {
                    if (handler->m_onTimeIncrease) handler->m_onTimeIncrease();
                } else {
                    if (handler->m_onTimeDecrease) handler->m_onTimeDecrease();
                }
                break;
        }
    }
}

void InputHandler::processInput(GLFWwindow* window, float deltaTime) {
    bool ctrlHeld = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    // Arrow keys - Rotation or Panning based on CTRL modifier
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanLeft) m_onPanLeft();
        else if (m_onRotateLeft) m_onRotateLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanRight) m_onPanRight();
        else if (m_onRotateRight) m_onRotateRight();
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanForward) m_onPanForward();
        else if (m_onRotateUp) m_onRotateUp();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanBackward) m_onPanBackward();
        else if (m_onRotateDown) m_onRotateDown();
    }

    // Page Up/Down for vertical panning (requires CTRL)
    if (ctrlHeld) {
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS && m_onPanUp) {
            m_onPanUp();
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS && m_onPanDown) {
            m_onPanDown();
        }
    }

    // Alternative WASD controls (same as arrows)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanLeft) m_onPanLeft();
        else if (m_onRotateLeft) m_onRotateLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanRight) m_onPanRight();
        else if (m_onRotateRight) m_onRotateRight();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanForward) m_onPanForward();
        else if (m_onRotateUp) m_onRotateUp();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (ctrlHeld && m_onPanBackward) m_onPanBackward();
        else if (m_onRotateDown) m_onRotateDown();
    }
}

void InputHandler::onCameraSwitch(int cameraIndex, KeyCallback callback) {
    int key = GLFW_KEY_F1 + (cameraIndex - 1);  // F1=camera1, F2=camera2, F3=camera3
    m_cameraSwitchCallbacks[key] = callback;
}