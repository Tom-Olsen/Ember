#include "cameraController.h"



// Constructors/Destructor:
CameraController::CameraController()
{
	m_moveSpeed = 6.5f;
	m_fastMoveMultiplier = 2.0f;
	m_rotationSpeed = 1.5f;
	m_zoomSpeed = 1.0f;
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
	float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? m_moveSpeed * m_fastMoveMultiplier : m_moveSpeed;

	Float3 direction = Float3::zero;
	if (EventSystem::KeyDownOrHeld(SDLK_W)) direction -= m_pTransform->GetForward();
	if (EventSystem::KeyDownOrHeld(SDLK_S)) direction += m_pTransform->GetForward();
	if (EventSystem::KeyDownOrHeld(SDLK_A)) direction -= m_pTransform->GetRight();
	if (EventSystem::KeyDownOrHeld(SDLK_D)) direction += m_pTransform->GetRight();
	if (EventSystem::KeyDownOrHeld(SDLK_Q)) direction -= m_pTransform->GetUp();
	if (EventSystem::KeyDownOrHeld(SDLK_E)) direction += m_pTransform->GetUp();

	m_pTransform->AddToPosition(direction * currentSpeed * Timer::GetDeltaTime());
}
void CameraController::Rotation()
{
	if (EventSystem::MouseDown(EventSystem::MouseButton::right))
	{
		m_mousePosOnDown = EventSystem::MousePos();
		m_rotationMatrixOnDown = m_pTransform->GetRotation3x3();
	}

	if (EventSystem::MouseHeld(EventSystem::MouseButton::right))
	{
		Float2 mousePos = EventSystem::MousePos();
		Float2 delta = 0.001f * m_rotationSpeed * (mousePos - m_mousePosOnDown);

		// Rotate around global Y-axis and local X-axis (note matrix multiplication order):
		Float3x3 rotY = Float3x3::RotateY(-delta.x);
		Float3x3 rotX = Float3x3::RotateX(-delta.y);
		m_pTransform->SetRotationMatrix(rotY * m_rotationMatrixOnDown * rotX);
	}
}
void CameraController::Zoom()
{
	float mouseScroll = EventSystem::MouseScrollY();
	if (mouseScroll != 0)
	{
		float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? m_zoomSpeed * m_fastMoveMultiplier : m_zoomSpeed;
		m_pTransform->AddToPosition(mouseScroll * currentSpeed * m_pTransform->GetBackward());
	}
}

// Overrides:
const std::string CameraController::ToString() const
{
	return "CameraController";
}