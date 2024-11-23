#pragma once
#ifndef __INCLUDE_GUARD_spotLight_h__
#define __INCLUDE_GUARD_spotLight_h__
#include "mathf.h"
#include "component.h"



class SpotLight : public Component
{
public: // Members:

private: // Members:
	float intensity;
	Float3 color;
	float fovRadians;
	float aspectRatio;
	float nearClip;
	float farClip;
	float blendStart;
	float blendEnd;
	Float4x4 projectionMatrix;
	bool updateProjectionMatrix;

public: // Methods:
	SpotLight();
	~SpotLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetColor(const Float3& color);
	void SetFovDegrees(const float& fovDegrees);
	void SetFovRadians(const float& fovRadians);
	void SetNearClip(const float& nearClip);
	void SetFarClip(const float& farClip);
	void SetBlendStart(const float& blendStart);
	void SetBlendEnd(const float& blendEnd);

	// Getters:
	Float3 GetDirection() const;
	float GetIntensity() const;
	Float3 GetColor() const;
	Float4 GetColorIntensity() const;
	float GetFovDegrees() const;
	float GetFovRadians() const;
	float GetNearClip() const;
	float GetFarClip() const;
	float GetBlendStart() const;
	float GetBlendEnd() const;
	Float2 GetBlendStartEnd() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__