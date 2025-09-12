#pragma once
#include "Utilities/ChiliWin.h"
#include <DirectXMath.h>

// Forward declarations
class Window;
class Mouse;
class Keyboard;

/**
 * @brief Movement directions for camera navigation
 */
enum class CameraDirection : unsigned char
{
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/**
 * @brief A free-flying camera implementation for 3D navigation
 * 
 * This camera allows full 6-DOF (degrees of freedom) movement in 3D space,
 * commonly used for first-person style navigation. It supports:
 * - WASD + QE movement (forward/back, strafe, up/down)
 * - Mouse look with pitch/yaw rotation
 * - Zoom functionality via mouse wheel
 * - Automatic mouse capture when active
 * 
 * The camera uses a right-handed coordinate system and integrates with
 * DirectX's left-handed rendering system through proper matrix generation.
 * 
 * @note This camera automatically handles mouse capture/release when toggled
 */
class FreeFlyCamera
{
public:

    /**
     * @brief Constructs a FreeFlyCamera with specified parameters
     * 
     * @param position Initial camera position in world space
     * @param worldUp World up vector (typically (0,1,0) for Y-up)
     * @param yaw Initial yaw rotation in degrees
     * @param pitch Initial pitch rotation in degrees
     */
    explicit FreeFlyCamera(
        DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f },
        DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f },
        float yaw = 90.0f,
        float pitch = 0.0f);

    /**
     * @brief Gets the view matrix for rendering
     * @return View matrix in DirectX left-handed coordinate system
     */
    DirectX::XMMATRIX GetViewMatrix() const noexcept;

    /**
     * @brief Processes all input and updates camera state
     * 
     * This method should be called once per frame. It handles:
     * - Right-click to toggle camera mode
     * - WASD/QE movement when active
     * - Mouse look when active
     * - Mouse wheel zoom
     * 
     * @param wnd Window for mouse capture control
     * @param mouse Mouse input state
     * @param keyboard Keyboard input state  
     * @param deltaTime Time elapsed since last frame (seconds)
     */
    void ProcessInput(Window& wnd, Mouse& mouse, const Keyboard& keyboard, float deltaTime) noexcept;

    /**
     * @brief Resets camera to default position and orientation
     */
    void Reset() noexcept;

    // === Property Accessors ===
    
    /**
     * @brief Gets current camera position
     * @return Position in world space
     */
    DirectX::XMFLOAT3 GetPosition() const noexcept;
    
    /**
     * @brief Sets camera position
     * @param position New position in world space
     */
    void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
    
    /**
     * @brief Gets current zoom level
     * @return Current zoom factor
     */
    float GetZoom() const noexcept;
    
    /**
     * @brief Gets current movement speed
     * @return Movement speed in units per second
     */
    float GetSpeed() const noexcept;
    
    /**
     * @brief Sets movement speed
     * @param speed New movement speed (must be positive)
     */
    void SetSpeed(float speed) noexcept;
    
    /**
     * @brief Gets current mouse sensitivity
     * @return Mouse sensitivity multiplier
     */
    float GetSensitivity() const noexcept;
    
    /**
     * @brief Sets mouse sensitivity
     * @param sensitivity New sensitivity (must be positive)
     */
    void SetSensitivity(float sensitivity) noexcept;
    
    /**
     * @brief Checks if camera is currently active (capturing mouse)
     * @return true if camera is active, false otherwise
     */
    bool IsActive() const noexcept;
    
    /**
     * @brief Gets the front direction vector
     * @return Normalized front vector in world space
     */
    DirectX::XMFLOAT3 GetFront() const noexcept;
    
    /**
     * @brief Gets the up direction vector  
     * @return Normalized up vector in world space
     */
    DirectX::XMFLOAT3 GetUp() const noexcept;
    
    /**
     * @brief Gets the right direction vector
     * @return Normalized right vector in world space
     */
    DirectX::XMFLOAT3 GetRight() const noexcept;

    /// <summary>
	/// Gets the near plane distance for projection
    /// </summary>
    /// <returns></returns>
    float GetNearPlane() const noexcept;

	/// <summary>
	/// Retrieves the value of the far clipping plane.
	/// </summary>
	/// <returns>The distance to the far clipping plane as a float.</returns>
	float GetFarPlane() const noexcept;

	/// <summary>
	/// Returns the aspect ratio as a floating-point value.
	/// </summary>
	/// <returns>The aspect ratio, typically defined as width divided by height.</returns>
	float GetAspectRatio() const noexcept;

	/// <summary>
	/// Retrieves the field of view (FOV) in degrees.
	/// </summary>
	/// <returns>The field of view angle, measured in degrees.</returns>
	float GetFovDegrees() const noexcept;
private:
    /**
     * @brief Updates movement based on keyboard input
     * @param direction Movement direction
     * @param deltaTime Frame time in seconds
     */
    void UpdateMovement(CameraDirection direction, float deltaTime) noexcept;
    
    /**
     * @brief Updates camera orientation from mouse input
     * @param xOffset Mouse X-axis movement
     * @param yOffset Mouse Y-axis movement
     * @param constrainPitch Whether to limit pitch rotation
     */
    void UpdateOrientation(float xOffset, float yOffset, bool constrainPitch = true) noexcept;
    
    /**
     * @brief Updates zoom level from mouse wheel
     * @param yOffset Mouse wheel delta
     */
    void UpdateZoom(float yOffset) noexcept;
    
    /**
     * @brief Toggles camera active state and mouse capture
     * @param wnd Window to control mouse capture
     */
    void ToggleActiveState(Window& wnd);
    
    /**
     * @brief Recalculates camera direction vectors from yaw/pitch
     */
    void UpdateCameraVectors() noexcept;

    // === Core State ===
    DirectX::XMFLOAT3 position;        ///< Camera position in world space
    DirectX::XMFLOAT3 front;           ///< Forward direction vector (calculated)
    DirectX::XMFLOAT3 up;              ///< Up direction vector (calculated) 
    DirectX::XMFLOAT3 right;           ///< Right direction vector (calculated)
    DirectX::XMFLOAT3 worldUp;         ///< World up vector (typically 0,1,0)
    
    // === Orientation ===
    float yaw;                          ///< Yaw rotation in degrees
    float pitch;                        ///< Pitch rotation in degrees
    
    // === Configuration ===
    float speed;                        ///< Movement speed (units/second)
    float sensitivity;                  ///< Mouse sensitivity multiplier
    float zoom;                         ///< Current zoom level
    
    // === Input State ===
    bool isActive = false;              ///< Whether camera is capturing mouse
    bool previousRightMouseState = false; ///< Previous right mouse button state

	// === Projection Parameters ===
	float nearPlane = 0.1f; ///< Near clipping plane distance
	float farPlane = 10000.0f;///< Far clipping plane 
	float fovDegrees = 45.0f; ///< Base field of view in degrees
	float aspectRatio = 16.0f / 9.0f; ///< Viewport aspect ratio
};

/**
 * @brief Creates a perspective projection matrix with camera's zoom
 * 
 * This is a utility function that creates a projection matrix using the camera's
 * current zoom level. The projection matrix should be managed separately from
 * the camera as it's typically constant across multiple cameras.
 * 
 * @param camera Camera to get zoom level from
 * @param fovDegrees Base field of view in degrees (before zoom)
 * @param aspectRatio Viewport aspect ratio (width/height)
 * @param nearZ Near clipping plane distance
 * @param farZ Far clipping plane distance
 * @return Perspective projection matrix
 */
DirectX::XMMATRIX CreateProjectionMatrix(
    const FreeFlyCamera& camera) noexcept;