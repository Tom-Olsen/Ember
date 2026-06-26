#pragma once
#include "component.h"
#include "emberMath.h"



namespace emberEcs
{
	class CameraController : public Component
	{
	private: // Members:
		bool m_isActive;
		bool m_isNavigating;
		float m_moveSpeed;
		float m_fastMoveMultiplier;
		float m_rotationSpeed;
		float m_zoomSpeed;

		Float2 m_mousePosOnDown;
		Float3x3 m_rotationMatrixOnDown;


	public: // Methods:
		CameraController();
		~CameraController();
		void SetIsActive(bool isActive);

		// Overrides:
		void Update() override;

	private: // Methods:
		void Translation();
		void Rotation();
		void Zoom();
		void TogglePerspectiveType();
		void UpdateNavigationLock();
		void CancelNavigation();
	};
}