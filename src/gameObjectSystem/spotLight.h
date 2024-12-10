#ifndef __INCLUDE_GUARD_spotLight_h__
#define __INCLUDE_GUARD_spotLight_h__
#include "emberEngine.h"



class SpotLight : public Component
{
private: // Members:
	float m_intensity;
	Float3 m_color;
	float m_fovRadians;
	float m_aspectRatio;
	float m_nearClip;
	float m_farClip;
	float m_blendStart;
	float m_blendEnd;
	Float4x4 m_projectionMatrix;
	bool m_updateProjectionMatrix;

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
	Float3 GetPosition() const;
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
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_spotLight_h__