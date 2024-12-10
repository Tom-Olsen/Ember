#ifndef __INCLUDE_GUARD_directionalLight_h__
#define __INCLUDE_GUARD_directionalLight_h__
#include "emberEngine.h"



class DirectionalLight : public Component
{
public: // Members:

private: // Members:
	float m_intensity;
	Float3 m_color;
	float m_nearClip;
	float m_farClip;
	float m_viewWidth;
	float m_viewHeight;
	Float4x4 m_projectionMatrix;
	bool m_updateProjectionMatrix;

public: // Methods:
	DirectionalLight();
	~DirectionalLight();

	// Setters:
	void SetIntensity(float intensity);
	void SetColor(const Float3& color);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);
	void SetViewWidth(float viewWidth);
	void SetViewHeight(float viewHeight);

	// Getters:
	Float3 GetDirection() const;
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
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__