#include "Camera/FreeFlyCamera.h"

using namespace DirectX;

FreeFlyCamera::FreeFlyCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 up, float yaw, float pitch)
	: position(position),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	speed(DEFAULT_SPEED),
	sensitivity(DEFAULT_SENSITIVITY),
	zoom(DEFAULT_ZOOM)
{
	front = { 0.0f, 0.0f, 1.0f };
	right = { 1.0f, 0.0f, 0.0f };
	this->up = { 0.0f, 1.0f, 0.0f };
	UpdateCameraVectors();
}

DirectX::XMMATRIX FreeFlyCamera::GetViewMatrix() const noexcept
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMFLOAT3 targetPosition;
	XMStoreFloat3(&targetPosition,
		XMVectorAdd(XMLoadFloat3(&position),XMLoadFloat3(&front)));
	XMVECTOR target = XMLoadFloat3(&targetPosition);

	XMVECTOR upDir = XMLoadFloat3(&this->up);
	
	return XMMatrixLookAtLH(pos, target, upDir); 
}

DirectX::XMMATRIX FreeFlyCamera::GetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ) const noexcept
{
	float fovRadians = XMConvertToRadians(fovDegrees) / zoom;
	return XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void FreeFlyCamera::UpdatePosition(CameraDirection direction, float deltaTime) noexcept
{
	float deltaPosition = speed * deltaTime; // speed/second * second

	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR front = XMLoadFloat3(&this->front);
	XMVECTOR right = XMLoadFloat3(&this->right);
	XMVECTOR up = XMLoadFloat3(&this->up);

	if (direction == CameraDirection::FORWARD)
	{
		pos = XMVectorAdd(pos, XMVectorScale(front, deltaPosition));
	}
	if (direction == CameraDirection::BACKWARD)
	{
		pos = XMVectorSubtract(pos, XMVectorScale(front, deltaPosition));
	}
	if (direction == CameraDirection::LEFT)
	{
		pos = XMVectorAdd(pos, XMVectorScale(right, deltaPosition));
	}
	if (direction == CameraDirection::RIGHT)
	{
		pos = XMVectorSubtract(pos, XMVectorScale(right, deltaPosition));
	}
	if (direction == CameraDirection::UP)
	{
		pos = XMVectorAdd(pos, XMVectorScale(up, deltaPosition));
	}
	if (direction == CameraDirection::DOWN)
	{
		pos = XMVectorSubtract(pos, XMVectorScale(up, deltaPosition));
	}

	XMStoreFloat3(&position, pos);
}

void FreeFlyCamera::UpdateOrientation(float xOffset, float yOffset, bool constrainPitch) noexcept
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	UpdateCameraVectors();
}

void FreeFlyCamera::UpdateZoom(float yOffset) noexcept
{
	zoom -= yOffset;

	if (zoom < 1.0f) zoom = 1.0f;
	if (zoom > 45.0f) zoom = 45.0f;
}

void FreeFlyCamera::Reset() noexcept
{
	position = { 0.0f, 0.0f, 0.0f };
	worldUp = { 0.0f, 1.0f, 0.0f };
	front = { 0.0f, 0.0f, 1.0f };
	yaw = DEFAULT_YAW;
	pitch = DEFAULT_PITCH_VALUE;

	speed = DEFAULT_SPEED;
	sensitivity = DEFAULT_SENSITIVITY;
	zoom = DEFAULT_ZOOM;

	UpdateCameraVectors();
}

void FreeFlyCamera::ProcessInput(Window& wnd, Mouse& mouse, const Keyboard& keyboard, float deltaTime) noexcept
{
	// Handle camera mode toggle
	bool currentRightMouseState = mouse.RightIsPressed();
	if (currentRightMouseState && !previousRightMouseState)
	{
		ToggleCameraMode(wnd);
	}
	previousRightMouseState = currentRightMouseState;

	// Only process movement input if camera is active
	if (!cameraActive)
		return;

	// Keyboard movement
	if (keyboard.KeyIsPressed('W'))
		UpdatePosition(CameraDirection::FORWARD, deltaTime);
	if (keyboard.KeyIsPressed('S'))
		UpdatePosition(CameraDirection::BACKWARD, deltaTime);
	if (keyboard.KeyIsPressed('A'))
		UpdatePosition(CameraDirection::LEFT, deltaTime);
	if (keyboard.KeyIsPressed('D'))
		UpdatePosition(CameraDirection::RIGHT, deltaTime);
	if (keyboard.KeyIsPressed('E') || keyboard.KeyIsPressed(VK_SPACE))
		UpdatePosition(CameraDirection::UP, deltaTime);
	if (keyboard.KeyIsPressed('Q') || keyboard.KeyIsPressed(VK_CONTROL))
		UpdatePosition(CameraDirection::DOWN, deltaTime);

	// Mouse orientation - process the deltas directly
	const int deltaX = mouse.GetDeltaX();
	const int deltaY = mouse.GetDeltaY();

	if (deltaX != 0 || deltaY != 0)
	{
		// Negating both x and y deltas so that camera matrix matches what the models must transform by.
		UpdateOrientation(static_cast<float>(-deltaX), static_cast<float>(-deltaY));
		// Reset mouse deltas after processing
		mouse.ClearDelta();
	}

	// Process wheel events from the queue
	while (!mouse.IsEmpty())
	{
		const auto e = mouse.Read();
		if (e.GetType() == Mouse::Event::Type::WheelUp)
		{
			UpdateZoom(-0.05f);
		}
		else if (e.GetType() == Mouse::Event::Type::WheelDown)
		{
			UpdateZoom(0.05f);
		}
	}
}

DirectX::XMFLOAT3 FreeFlyCamera::GetPosition() const noexcept
{
	return position;
}

void FreeFlyCamera::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	this->position = position;
}

float FreeFlyCamera::GetZoom() const noexcept
{
	return zoom;
}

void FreeFlyCamera::ToggleCameraMode(Window& wnd)
{
	cameraActive = !cameraActive;
	if (cameraActive)
	{
		wnd.CaptureMouse();
	}
	else
	{
		wnd.ReleaseMouse();
	}
}

bool FreeFlyCamera::IsCameraActive() const noexcept
{
	return cameraActive;
}

void FreeFlyCamera::UpdateCameraVectors() noexcept
{
	// Calculate new front vector based on yaw and pitch
	DirectX::XMFLOAT3 newFront;
	newFront.x = cosf(DirectX::XMConvertToRadians(yaw)) * cosf(DirectX::XMConvertToRadians(pitch));
	newFront.y = sinf(DirectX::XMConvertToRadians(pitch));
	newFront.z = sinf(DirectX::XMConvertToRadians(yaw)) * cosf(DirectX::XMConvertToRadians(pitch));

	// Normalize front vector
	DirectX::XMVECTOR frontVector = DirectX::XMLoadFloat3(&newFront);
	frontVector = DirectX::XMVector3Normalize(frontVector);
	DirectX::XMStoreFloat3(&front, frontVector);

	// Calculate right vector
	DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&worldUp);
	DirectX::XMVECTOR rightVector = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(frontVector, upVector)
	);
	DirectX::XMStoreFloat3(&right, rightVector);

	// Calculate up vector
	upVector = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(rightVector, frontVector)
	);
	DirectX::XMStoreFloat3(&up, upVector);
}
