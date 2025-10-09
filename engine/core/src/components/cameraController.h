#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class CameraController : public Component
	{
	private: // Members:
		bool m_isActive;
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
		const std::string ToString() const override;

	private: // Methods:
		void Translation();
		void Rotation();
		void Zoom();
		void TogglePerspectiveType();
	};
}