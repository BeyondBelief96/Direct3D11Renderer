#include "Camera/FreeFlyCamera.h"
#include "Core/Window.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include <algorithm>
#include <cmath>

using namespace DirectX;


FreeFlyCamera::FreeFlyCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 worldUp, float yaw, float pitch)
	: position(position),
	worldUp(worldUp),
	yaw(yaw),
	pitch(pitch),
	speed(10.0f),
	sensitivity(0.1f),
	zoom(1.0f)
{
	// Initialize vectors - these will be properly calculated by UpdateCameraVectors
	front = { 0.0f, 0.0f, 1.0f };
	right = { 1.0f, 0.0f, 0.0f };
	up = { 0.0f, 1.0f, 0.0f };
	
	UpdateCameraVectors();
}

DirectX::XMMATRIX FreeFlyCamera::GetViewMatrix() const noexcept
{
	const XMVECTOR pos = XMLoadFloat3(&position);
	const XMVECTOR target = XMVectorAdd(pos, XMLoadFloat3(&front));
	const XMVECTOR upDir = XMLoadFloat3(&up);
	
	return XMMatrixLookAtLH(pos, target, upDir);
}

void FreeFlyCamera::ProcessInput(Window& wnd, Mouse& mouse, const Keyboard& keyboard, float deltaTime) noexcept
{
	// Handle camera mode toggle
	const bool currentRightMouseState = mouse.RightIsPressed();
	if (currentRightMouseState && !previousRightMouseState)
	{
		ToggleActiveState(wnd);
	}
	previousRightMouseState = currentRightMouseState;

	// Only process movement input if camera is active
	if (!isActive)
		return;

	// Process keyboard movement
	if (keyboard.KeyIsPressed('W'))
		UpdateMovement(CameraDirection::FORWARD, deltaTime);
	if (keyboard.KeyIsPressed('S'))
		UpdateMovement(CameraDirection::BACKWARD, deltaTime);
	if (keyboard.KeyIsPressed('A'))
		UpdateMovement(CameraDirection::LEFT, deltaTime);
	if (keyboard.KeyIsPressed('D'))
		UpdateMovement(CameraDirection::RIGHT, deltaTime);
	if (keyboard.KeyIsPressed('E') || keyboard.KeyIsPressed(VK_SPACE))
		UpdateMovement(CameraDirection::UP, deltaTime);
	if (keyboard.KeyIsPressed('Q') || keyboard.KeyIsPressed(VK_CONTROL))
		UpdateMovement(CameraDirection::DOWN, deltaTime);

	// Process mouse orientation
	const int deltaX = mouse.GetDeltaX();
	const int deltaY = mouse.GetDeltaY();

	if (deltaX != 0 || deltaY != 0)
	{
		// Negate deltas to match expected camera behavior
		UpdateOrientation(static_cast<float>(-deltaX), static_cast<float>(-deltaY));
		// Clear deltas after processing
		mouse.ClearDelta();
	}

	// Process mouse wheel zoom
	while (!mouse.IsEmpty())
	{
		const auto event = mouse.Read();
		switch (event.GetType())
		{
		case Mouse::Event::Type::WheelUp:
			UpdateZoom(-0.05f);
			break;
		case Mouse::Event::Type::WheelDown:
			UpdateZoom(0.05f);
			break;
		default:
			break;
		}
	}
}

void FreeFlyCamera::Reset() noexcept
{
	position = { 0.0f, 0.0f, 0.0f };
	worldUp = { 0.0f, 1.0f, 0.0f };
	yaw = 90.0f;
	pitch = 0.0f;
	speed = 10.0f;
	sensitivity = 0.1f;
	zoom = 1.0f;
	
	UpdateCameraVectors();
}

// === Property Accessors ===

DirectX::XMFLOAT3 FreeFlyCamera::GetPosition() const noexcept
{
	return position;
}

void FreeFlyCamera::SetPosition(const DirectX::XMFLOAT3& newPosition) noexcept
{
	position = newPosition;
}

float FreeFlyCamera::GetZoom() const noexcept
{
	return zoom;
}

float FreeFlyCamera::GetSpeed() const noexcept
{
	return speed;
}

void FreeFlyCamera::SetSpeed(float newSpeed) noexcept
{
	speed = std::max(0.1f, newSpeed); // Ensure positive speed
}

float FreeFlyCamera::GetSensitivity() const noexcept
{
	return sensitivity;
}

void FreeFlyCamera::SetSensitivity(float newSensitivity) noexcept
{
	sensitivity = std::max(0.01f, newSensitivity); // Ensure positive sensitivity
}

bool FreeFlyCamera::IsActive() const noexcept
{
	return isActive;
}

DirectX::XMFLOAT3 FreeFlyCamera::GetFront() const noexcept
{
	return front;
}

DirectX::XMFLOAT3 FreeFlyCamera::GetUp() const noexcept
{
	return up;
}

DirectX::XMFLOAT3 FreeFlyCamera::GetRight() const noexcept
{
	return right;
}

float FreeFlyCamera::GetNearPlane() const noexcept
{
	return nearPlane;
}

float FreeFlyCamera::GetFarPlane() const noexcept
{
	return farPlane;
}

float FreeFlyCamera::GetAspectRatio() const noexcept
{
	return aspectRatio;
}

float FreeFlyCamera::GetFovDegrees() const noexcept
{
	return fovDegrees;
}

// === Private Methods ===

void FreeFlyCamera::UpdateMovement(CameraDirection direction, float deltaTime) noexcept
{
	const float deltaPosition = speed * deltaTime;
	
	XMVECTOR pos = XMLoadFloat3(&position);
	const XMVECTOR frontVec = XMLoadFloat3(&front);
	const XMVECTOR rightVec = XMLoadFloat3(&right);
	const XMVECTOR upVec = XMLoadFloat3(&up);

	switch (direction)
	{
	case CameraDirection::FORWARD:
		pos = XMVectorAdd(pos, XMVectorScale(frontVec, deltaPosition));
		break;
	case CameraDirection::BACKWARD:
		pos = XMVectorSubtract(pos, XMVectorScale(frontVec, deltaPosition));
		break;
	case CameraDirection::LEFT:
		pos = XMVectorAdd(pos, XMVectorScale(rightVec, deltaPosition));
		break;
	case CameraDirection::RIGHT:
		pos = XMVectorSubtract(pos, XMVectorScale(rightVec, deltaPosition));
		break;
	case CameraDirection::UP:
		pos = XMVectorAdd(pos, XMVectorScale(upVec, deltaPosition));
		break;
	case CameraDirection::DOWN:
		pos = XMVectorSubtract(pos, XMVectorScale(upVec, deltaPosition));
		break;
	default:
		break;
	}

	XMStoreFloat3(&position, pos);
}

void FreeFlyCamera::UpdateOrientation(float xOffset, float yOffset, bool constrainPitch) noexcept
{
	// Apply sensitivity
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	// Update angles
	yaw += xOffset;
	pitch += yOffset;

	// Constrain pitch to prevent camera flipping
	if (constrainPitch)
	{
		pitch = std::clamp(pitch, -89.0f, 89.0f);
	}

	// Recalculate camera vectors
	UpdateCameraVectors();
}

void FreeFlyCamera::UpdateZoom(float yOffset) noexcept
{
	zoom -= yOffset;
	zoom = std::clamp(zoom, 1.0f, 45.0f);
}

void FreeFlyCamera::ToggleActiveState(Window& wnd)
{
	isActive = !isActive;
	
	if (isActive)
	{
		wnd.CaptureMouse();
	}
	else
	{
		wnd.ReleaseMouse();
	}
}

void FreeFlyCamera::UpdateCameraVectors() noexcept
{
	// Calculate new front vector from yaw and pitch
	const float yawRad = XMConvertToRadians(yaw);
	const float pitchRad = XMConvertToRadians(pitch);
	
	DirectX::XMFLOAT3 newFront;
	newFront.x = std::cos(yawRad) * std::cos(pitchRad);
	newFront.y = std::sin(pitchRad);
	newFront.z = std::sin(yawRad) * std::cos(pitchRad);

	// Normalize front vector
	XMVECTOR frontVector = XMLoadFloat3(&newFront);
	frontVector = XMVector3Normalize(frontVector);
	XMStoreFloat3(&front, frontVector);

	// Calculate right vector (cross product of front and world up)
	const XMVECTOR worldUpVector = XMLoadFloat3(&worldUp);
	XMVECTOR rightVector = XMVector3Normalize(XMVector3Cross(frontVector, worldUpVector));
	XMStoreFloat3(&right, rightVector);

	// Calculate up vector (cross product of right and front)
	XMVECTOR upVector = XMVector3Normalize(XMVector3Cross(rightVector, frontVector));
	XMStoreFloat3(&up, upVector);
}

// === Global Utility Function ===

DirectX::XMMATRIX CreateProjectionMatrix(
	const FreeFlyCamera& camera) noexcept
{
	const float fovRadians = XMConvertToRadians(camera.GetFovDegrees()) / camera.GetZoom();
	return XMMatrixPerspectiveFovLH(fovRadians, camera.GetAspectRatio(), camera.GetNearPlane(), camera.GetFarPlane());
}