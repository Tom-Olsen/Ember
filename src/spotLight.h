#pragma once
#ifndef __INCLUDE_GUARD_spotLight_h__
#define __INCLUDE_GUARD_spotLight_h__
#include "glmTypes.h"
#include "component.h"
#include "gameObject.h"



class SpotLight : public Component
{
public: // Members:

private: // Members:
	float intensity;
	float fov;
	float aspectRatio;
	float nearClip;
	float farClip;
	Float4x4 projectionMatrix;
	bool updateProjectionMatrix;

public: // Methods:
	SpotLight();
	~SpotLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetFov(const float& fov);

	// Getters:
	Float4x4 GetViewMatrix();
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__