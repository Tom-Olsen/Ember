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
		float currentSpeed = EventSystem::KeyDownOrHeld(Input::Key::ShiftLeft) ? m_moveSpeed * m_fastMoveMultiplier : m_moveSpeed;

		Float3 direction = Float3::zero;
		Transform* transform = GetTransform();
		if (EventSystem::KeyDownOrHeld(Input::Key::W)) direction -= transform->GetUp();
		if (EventSystem::KeyDownOrHeld(Input::Key::S)) direction += transform->GetUp();
		if (EventSystem::KeyDownOrHeld(Input::Key::A)) direction -= transform->GetRight();
		if (EventSystem::KeyDownOrHeld(Input::Key::D)) direction += transform->GetRight();
		if (EventSystem::KeyDownOrHeld(Input::Key::Q)) direction -= transform->GetForward();
		if (EventSystem::KeyDownOrHeld(Input::Key::E)) direction += transform->GetForward();

		transform->AddToPosition(direction * currentSpeed * Time::GetDeltaTime());
	}
	void CameraController::Rotation()
	{
		if (EventSystem::MouseDown(Input::MouseButton::Right))
		{
			m_mousePosOnDown = EventSystem::MousePos();
			m_rotationMatrixOnDown = GetTransform()->GetRotation3x3();
		}

		if (EventSystem::MouseHeld(Input::MouseButton::Right))
		{
			Float2 mousePos = EventSystem::MousePos();
			Float2 delta = 0.001f * m_rotationSpeed * (mousePos - m_mousePosOnDown);

			// Rotate around global Z-axis and local X-axis (note matrix multiplication order):
			Float3x3 rotZ = Float3x3::RotateZ(-delta.x);
			Float3x3 rotX = Float3x3::RotateX(-delta.y);
			GetTransform()->SetRotationMatrix(rotZ * m_rotationMatrixOnDown * rotX);
		}
	}
	void CameraController::Zoom()
	{
		float mouseScroll = EventSystem::MouseScrollY();
		if (mouseScroll != 0)
		{
			float currentSpeed = EventSystem::KeyDownOrHeld(Input::Key::ShiftLeft) ? m_zoomSpeed * m_fastMoveMultiplier : m_zoomSpeed;
			Transform* transform = GetTransform();
			transform->AddToPosition(mouseScroll * currentSpeed * transform->GetDown());
		}
	}
	void CameraController::TogglePerspectiveType()
	{
		if (EventSystem::KeyDown(Input::Key::P))
		{
			Camera* pCamera = GetComponent<Camera>();
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
		if (EventSystem::KeyDown(Input::Key::Tab))
			m_isActive = !m_isActive;
	}
}