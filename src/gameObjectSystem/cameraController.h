#pragma once
#ifndef __INCLUDE_GUARD_cameraController_h__
#define __INCLUDE_GUARD_cameraController_h__
#include "mathf.h"
#include "component.h"



/// <summary>
/// Default view direction is -z = transform->backward
/// </summary>
class CameraController : public Component
{
public: // Members:
	float moveSpeed;
	float fastMoveMultiplier;
	float rotationSpeed;
	float zoomSpeed;

	Float3 currentRotation;
	Float2 mousePosOnDown;
	Float3x3 rotationMatrixOnDown;


public: // Methods:
	CameraController();
	~CameraController();

	// Overrides:
	void Update() override;
	std::string ToString() const override;

private: // Methods:
	void Translation();
	void Rotation();
	void Zoom();
};



#endif // __INCLUDE_GUARD_cameraController_h__