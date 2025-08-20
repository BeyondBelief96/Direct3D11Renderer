#pragma once
#include "Utilities/ChiliWin.h"
#include "Core/Window.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include <DirectXMath.h>

enum class CameraDirection
{
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class FreeFlyCamera
{
public:
    // Constants
    static constexpr float DEFAULT_YAW = 90.0f;
    static constexpr float DEFAULT_PITCH_VALUE = 0.0f;
    static constexpr float DEFAULT_SPEED = 10.0f;
    static constexpr float DEFAULT_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_ZOOM = 1.0f;

    FreeFlyCamera(
        DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f },
        DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f },
        float yaw = DEFAULT_YAW,
        float pitch = DEFAULT_PITCH_VALUE);

    DirectX::XMMATRIX GetViewMatrix() const noexcept;
    DirectX::XMMATRIX GetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ) const noexcept;

    // Update camera position from keyboard input
    void UpdatePosition(CameraDirection direction, float deltaTime) noexcept;
    // Update camera orientation from mouse input
    void UpdateOrientation(float xOffset, float yOffset, bool constrainPitch = true) noexcept;
    // Update zoom level from mouse scroll input
    void UpdateZoom(float yOffset) noexcept;
    void Reset() noexcept;
    void ProcessInput(Window& wnd, Mouse& mouse, const Keyboard& keyboard, float deltaTime) noexcept;
    DirectX::XMFLOAT3 GetPosition() const noexcept;
    void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
    float GetZoom() const noexcept;

    void ToggleCameraMode(Window& wnd);
    bool IsCameraActive() const noexcept;

private:
    // Camera attributes
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 front;
    DirectX::XMFLOAT3 up;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 worldUp;
    // Euler Angles
    float yaw;
    float pitch;
    // Camera options
    float speed;
    float sensitivity;
    float zoom;
    bool cameraActive = false;
    // Mouse tracking
    bool firstMouseMovement = true;
    bool previousRightMouseState = false;
    float lastX;
    float lastY;
    void UpdateCameraVectors() noexcept;
};