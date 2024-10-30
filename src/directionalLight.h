#pragma once
#ifndef __INCLUDE_GUARD_directionalLight_h__
#define __INCLUDE_GUARD_directionalLight_h__
#include "glmTypes.h"
#include "component.h"
#include "gameObject.h"



/// <summary>
/// Default view direction is y (not the usual -z).
/// </summary>
class DirectionalLight : public Component
{
public: // Members:

private: // Members:
	static Float4x4 rotateToY;
	float intensity;
	Float4 color;
	float nearClip;
	float farClip;
	float viewWidth;
	float viewHeight;
	Float4x4 projectionMatrix;
	bool updateProjectionMatrix;

public: // Methods:
	DirectionalLight(const Float4& color = Float4(1.0f));
	~DirectionalLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetColor(const Float3& color);
	void SetColor(const Float4& color);
	void SetNearClip(const float& nearClip);
	void SetFarClip(const float& farClip);
	void SetViewWidth(const float& viewWidth);
	void SetViewHeight(const float& viewHeight);

	// Getters:
	Float4 GetColor() const;
	Float4x4 GetViewMatrix();
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__