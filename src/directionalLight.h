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
	float GetIntensity() const;
	Float4 GetColor() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__

//Sun position:
// (6, 6, 3)
// Sun view direction:
// (-0.666667, -0.666667, -0.333333)
//Sun localToWorld:
// (  0.447214, -0.596285, 0.666667, 6
// | -1.19209e-07, 0.745356, 0.666667, 6
// | -1.19209e-07, -0.298143, 0.333333, 3
// |  0, 0, 0, 1)
//Sun worldToLocal:
// (  0.447214, -7.45058e-08, -0.894427, -3.19872e-07
// | -0.596285, 0.745356, -0.298143, 4.26496e-07
// | -0.596285, 0.666667, 0.333333, -9
// | -0, 0, -0, 1)
//Sun projection:
// ( 0.133333, 0, 0, -0
// | 0, 0.133333, 0, -0
// | 0, 0, -0.133422, -1.00133
// | 0, 0, 0, 1)