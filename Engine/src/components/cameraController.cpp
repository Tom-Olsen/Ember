#include "cameraController.h"
#include "camera.h"



namespace emberEngine
{
	// Constructors/Destructor:
	CameraController::CameraController()
	{
		m_isActive = true;
		m_moveSpeed = 6.5f;
		m_fastMoveMultiplier = 2.0f;
		m_rotationSpeed = 1.5f;
		m_zoomSpeed = 1.0f;
	}
	CameraController::~CameraController()
	{

	}



	// Public methods:
	void CameraController::SetIsActive(bool isActive)
	{
		m_isActive = isActive;
	}


	// Private methods:
	void CameraController::Translation()
	{
		float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? m_moveSpeed * m_fastMoveMultiplier : m_moveSpeed;

		Float3 direction = Float3::zero;
		if (EventSystem::KeyDownOrHeld(SDLK_W)) direction -= m_pTransform->GetUp();
		if (EventSystem::KeyDownOrHeld(SDLK_S)) direction += m_pTransform->GetUp();
		if (EventSystem::KeyDownOrHeld(SDLK_A)) direction -= m_pTransform->GetRight();
		if (EventSystem::KeyDownOrHeld(SDLK_D)) direction += m_pTransform->GetRight();
		if (EventSystem::KeyDownOrHeld(SDLK_Q)) direction -= m_pTransform->GetForward();
		if (EventSystem::KeyDownOrHeld(SDLK_E)) direction += m_pTransform->GetForward();

		m_pTransform->AddToPosition(direction * currentSpeed * Time::GetDeltaTime());
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

			// Rotate around global Z-axis and local X-axis (note matrix multiplication order):
			Float3x3 rotZ = Float3x3::RotateZ(-delta.x);
			Float3x3 rotX = Float3x3::RotateX(-delta.y);
			m_pTransform->SetRotationMatrix(rotZ * m_rotationMatrixOnDown * rotX);
		}
	}
	void CameraController::Zoom()
	{
		float mouseScroll = EventSystem::MouseScrollY();
		if (mouseScroll != 0)
		{
			float currentSpeed = EventSystem::KeyDownOrHeld(SDLK_LSHIFT) ? m_zoomSpeed * m_fastMoveMultiplier : m_zoomSpeed;
			m_pTransform->AddToPosition(mouseScroll * currentSpeed * m_pTransform->GetDown());
		}
	}
	void CameraController::TogglePerspectiveType()
	{
		if (EventSystem::KeyDown(SDLK_P))
		{
			Camera* pCamera = GetGameObject()->GetComponent<Camera>();
			if (pCamera == nullptr)
				return;

			ProjectionType currentType = pCamera->GetProjectionType();
			if (currentType == ProjectionType::perspective)
				pCamera->SetProjectionType(ProjectionType::orthographic);
			else if (currentType == ProjectionType::orthographic)
				pCamera->SetProjectionType(ProjectionType::perspective);
		}
	}



	// Overrides:
	void CameraController::Update()
	{
		if (m_isActive)
		{
			Translation();
			Rotation();
			Zoom();
			TogglePerspectiveType();
		}

		// Toggle isActive:
		if (EventSystem::KeyDown(SDLK_TAB))
			m_isActive = !m_isActive;
	}
	const std::string CameraController::ToString() const
	{
		return "CameraController";
	}
}