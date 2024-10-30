#pragma once
#ifndef __INCLUDE_GUARD_spotLight_h__
#define __INCLUDE_GUARD_spotLight_h__
#include "mathf.h"
#include "component.h"
#include "gameObject.h"



class SpotLight : public Component
{
public: // Members:

private: // Members:
	static Float4x4 rotateToY;
	float intensity;
	float fovRadians;
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
	void SetFovDegrees(const float& fovDegrees);
	void SetFovRadians(const float& fovRadians);

	// Getters:
	Float4x4 GetViewMatrix();
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__