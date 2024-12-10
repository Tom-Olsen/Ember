#ifndef __INCLUDE_GUARD_cameraController_h__
#define __INCLUDE_GUARD_cameraController_h__
#include "emberEngine.h"



/// <summary>
/// Default view direction is -z = transform->backward
/// </summary>
class CameraController : public Component
{
private: // Members:
	float m_moveSpeed;
	float m_fastMoveMultiplier;
	float m_rotationSpeed;
	float m_zoomSpeed;

	Float2 m_mousePosOnDown;
	Float3x3 m_rotationMatrixOnDown;


public: // Methods:
	CameraController();
	~CameraController();

	// Overrides:
	void Update() override;
	const std::string ToString() const override;

private: // Methods:
	void Translation();
	void Rotation();
	void Zoom();
};



#endif // __INCLUDE_GUARD_cameraController_h__