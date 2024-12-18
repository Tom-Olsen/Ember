#ifndef __INCLUDE_GUARD_pointLight_h__
#define __INCLUDE_GUARD_pointLight_h__
#include "emberEngine.h"



class PointLight : public Component
{
public: // Members:

private: // Members:
	float m_intensity;
	Float3 m_color;
	float m_nearClip;
	float m_farClip;
	bool m_updateProjectionMatrix;
	Float4x4 m_projectionMatrix;
	static bool s_rotationMatricesInitialized;
	static Float4x4 s_rotationMatrices[6];
	bool m_drawFrustum;

public: // Methods:
	PointLight();
	~PointLight();

	// Setters:
	void SetIntensity(const float& intensity);
	void SetColor(const Float3& color = Float3::one);
	void SetNearClip(const float& nearClip);
	void SetFarClip(const float& farClip);
	void SetDrawFrustum(bool drawFrustum);

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
	void LateUpdate() override;
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_pointLight_h__