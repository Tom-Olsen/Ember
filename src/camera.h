#pragma once
#ifndef __INCLUDE_GUARD_camera_h__
#define __INCLUDE_GUARD_camera_h__
#include "glmTypes.h"
#include "component.h"



class Camera : public Component
{
public: // Members:
	float fov;
	float aspectRatio;
	float nearClip;
	float farClip;
	Float4x4 viewMatrix;
	Float4x4 projectionMatrix;
	bool updateCamera;

public: // Methods:
	Camera();
	~Camera();
	Float4x4 GetViewMatrix();
	Float4x4 GetProjectionMatrix();
	void PrintType() const;

private: // Methods:
	void UpdateViewMatrix();
};



#endif // __INCLUDE_GUARD_camera_h__