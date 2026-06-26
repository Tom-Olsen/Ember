#include "cameraController.h"
#include "camera.h"
#include "commonInput.h"
#include "component.inl"
#include "emberTime.h"
#include "entity.inl"
#include "eventSystem.h"
#include "transform.h"
using namespace emberCore;
using namespace emberCommon;



namespace emberEcs
{
	// Constructors/Destructor:
	CameraController::CameraController()
	{
		m_isActive = true;
		m_isNavigating = false;
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
		if (!m_isNavigating)
			return;

		bool fastMovement = EventSystem::KeyDownOrHeld(Input::Key::ShiftLeft);
		if (fastMovement)
			EventSystem::ConsumeKey(Input::Key::ShiftLeft);
		float currentSpeed = fastMovement ? m_moveSpeed * m_fastMoveMultiplier : m_moveSpeed;

		Float3 direction = Float3::zero;
		Transform* transform = GetTransform();
		if (EventSystem::KeyDownOrHeld(Input::Key::W))
		{
			direction -= transform->GetUp();
			EventSystem::ConsumeKey(Input::Key::W);
		}
		if (EventSystem::KeyDownOrHeld(Input::Key::S))
		{
			direction += transform->GetUp();
			EventSystem::ConsumeKey(Input::Key::S);
		}
		if (EventSystem::KeyDownOrHeld(Input::Key::A))
		{
			direction -= transform->GetRight();
			EventSystem::ConsumeKey(Input::Key::A);
		}
		if (EventSystem::KeyDownOrHeld(Input::Key::D))
		{
			direction += transform->GetRight();
			EventSystem::ConsumeKey(Input::Key::D);
		}
		if (EventSystem::KeyDownOrHeld(Input::Key::Q))
		{
			direction -= transform->GetForward();
			EventSystem::ConsumeKey(Input::Key::Q);
		}
		if (EventSystem::KeyDownOrHeld(Input::Key::E))
		{
			direction += transform->GetForward();
			EventSystem::ConsumeKey(Input::Key::E);
		}

		transform->AddToPosition(direction * currentSpeed * Time::GetDeltaTime());
	}
	void CameraController::Rotation()
	{
		if (!m_isNavigating)
			return;

		if (EventSystem::MouseDown(Input::MouseButton::Right) || EventSystem::MouseHeld(Input::MouseButton::Right))
		{
			EventSystem::ConsumeMouseButton(Input::MouseButton::Right);
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
			EventSystem::ConsumeMouseScroll();
			bool fastMovement = EventSystem::KeyDownOrHeld(Input::Key::ShiftLeft);
			if (fastMovement)
				EventSystem::ConsumeKey(Input::Key::ShiftLeft);
			float currentSpeed = fastMovement ? m_zoomSpeed * m_fastMoveMultiplier : m_zoomSpeed;
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

			Camera::ProjectionType currentType = pCamera->GetProjectionType();
			if (currentType == Camera::ProjectionType::perspective)
				pCamera->SetProjectionType(Camera::ProjectionType::orthographic);
			else if (currentType == Camera::ProjectionType::orthographic)
				pCamera->SetProjectionType(Camera::ProjectionType::perspective);
			EventSystem::ConsumeKey(Input::Key::P);
		}
	}
	void CameraController::UpdateNavigationLock()
	{
		if (EventSystem::MouseDown(Input::MouseButton::Right))
		{
			m_isNavigating = EventSystem::TryLockMouseButton(Input::MouseButton::Right);
			if (m_isNavigating)
			{
				EventSystem::TryLockKeyboard();
				EventSystem::ConsumeMouseButton(Input::MouseButton::Right);
				m_mousePosOnDown = EventSystem::MousePos();
				m_rotationMatrixOnDown = GetTransform()->GetRotation3x3();
			}
		}

		if (!m_isNavigating)
			return;

		if (EventSystem::MouseUp(Input::MouseButton::Right))
		{
			EventSystem::ConsumeMouseButton(Input::MouseButton::Right);
			EventSystem::UnlockKeyboard();
			m_isNavigating = false;
		}
		else if (!EventSystem::MouseDown(Input::MouseButton::Right) && !EventSystem::MouseHeld(Input::MouseButton::Right))
			CancelNavigation();
	}
	void CameraController::CancelNavigation()
	{
		EventSystem::UnlockKeyboard();
		EventSystem::UnlockMouseButton(Input::MouseButton::Right);
		m_isNavigating = false;
	}



	// Overrides:
	void CameraController::Update()
	{
		// Toggle isActive:
		if (EventSystem::KeyDown(Input::Key::Tab))
		{
			EventSystem::ConsumeKey(Input::Key::Tab);
			m_isActive = !m_isActive;
			if (!m_isActive)
				CancelNavigation();
		}

		if (!m_isActive)
			return;

		UpdateNavigationLock();
		Translation();
		Rotation();
		Zoom();
		TogglePerspectiveType();
	}
}