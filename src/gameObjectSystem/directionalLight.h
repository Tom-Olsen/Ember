#pragma once
#ifndef __INCLUDE_GUARD_directionalLight_h__
#define __INCLUDE_GUARD_directionalLight_h__
#include "mathf.h"
#include "component.h"



class DirectionalLight : public Component
{
public: // Members:

private: // Members:
	float intensity;
	Float3 color;
	float nearClip;
	float farClip;
	float viewWidth;
	float viewHeight;
	Float4x4 projectionMatrix;
	bool updateProjectionMatrix;

public: // Methods:
	DirectionalLight();
	~DirectionalLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetColor(const Float3& color);
	void SetNearClip(const float& nearClip);
	void SetFarClip(const float& farClip);
	void SetViewWidth(const float& viewWidth);
	void SetViewHeight(const float& viewHeight);

	// Getters:
	float GetIntensity() const;
	Float3 GetColor() const;
	Float4 GetColorIntensity() const;
	float GetNearClip() const;
	float GetFarClip() const;
	float GetViewWidth() const;
	float GetViewHeight() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__