#pragma once
#ifndef __INCLUDE_GUARD_pointLight_h__
#define __INCLUDE_GUARD_pointLight_h__
#include "mathf.h"
#include "component.h"



class PointLight : public Component
{
public: // Members:

private: // Members:
	float intensity;
	Float3 color;
	float nearClip;
	float farClip;
	Float4x4 projectionMatrix;
	static bool rotationMatricesInitialized;
	static Float4x4 rotationMatrices[6];
	bool updateProjectionMatrix;

public: // Methods:
	PointLight();
	~PointLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetColor(const Float3& color);
	void SetNearClip(const float& nearClip);
	void SetFarClip(const float& farClip);

	// Getters:
	Float3 GetPosition() const;
	float GetIntensity() const;
	Float3 GetColor() const;
	Float4 GetColorIntensity() const;
	float GetNearClip() const;
	float GetFarClip() const;
	Float4x4 GetViewMatrix(uint32_t faceIndex) const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_pointLight_h__