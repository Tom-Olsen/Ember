#include "cameraController.h"
#include "gameObject.h"
#include "eventSystem.h"
#include "logger.h"



// Constructors/Destructor:
CameraController::CameraController()
{
	moveSpeed = 6.5f;
	fastMoveMultiplier = 2.0f;
	rotationSpeed = 1.5f;
	zoomSpeed = 1.0f;
}
CameraController::~CameraController()
{

}



// Public methods:
void CameraController::Update()
{
	Translation();
	Rotation();
	Zoom();
}



// Private methods:
void CameraController::Translation()
{
	float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? moveSpeed * fastMoveMultiplier : moveSpeed;

	Float3 direction = Float3::zero;
	if (EventSystem::KeyDownOrHeld(SDLK_W)) direction -= transform->GetForward();
	if (EventSystem::KeyDownOrHeld(SDLK_S)) direction += transform->GetForward();
	if (EventSystem::KeyDownOrHeld(SDLK_A)) direction -= transform->GetRight();
	if (EventSystem::KeyDownOrHeld(SDLK_D)) direction += transform->GetRight();
	if (EventSystem::KeyDownOrHeld(SDLK_Q)) direction -= transform->GetUp();
	if (EventSystem::KeyDownOrHeld(SDLK_E)) direction += transform->GetUp();

	transform->AddToPosition(direction * currentSpeed * Timer::GetDeltaTime());
}
void CameraController::Rotation()
{
	if (EventSystem::MouseDown(EventSystem::MouseButton::right))
	{
		mousePosOnDown = EventSystem::MousePos();
		rotationMatrixOnDown = transform->GetRotation3x3();
	}

	if (EventSystem::MouseHeld(EventSystem::MouseButton::right))
	{
		Float2 mousePos = EventSystem::MousePos();
		Float2 delta = 0.001f * rotationSpeed * (mousePos - mousePosOnDown);

		// Rotate around global Y-axis and local X-axis (note matrix multiplication order):
		Float3x3 rotY = Float3x3::RotateY(-delta.x);
		Float3x3 rotX = Float3x3::RotateX(-delta.y);
		transform->SetRotationMatrix(rotY * rotationMatrixOnDown * rotX);
	}
}
void CameraController::Zoom()
{
	float mouseScroll = EventSystem::MouseScrollY();
	if (mouseScroll != 0)
	{
		float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? zoomSpeed * fastMoveMultiplier : zoomSpeed;
		transform->AddToPosition(mouseScroll * currentSpeed * transform->GetBackward());
	}
}

// Overrides:
std::string CameraController::ToString() const
{
	return "CameraController";
}