#ifndef __INCLUDE_GUARD_camera_h__
#define __INCLUDE_GUARD_camera_h__
#include "emberEngine.h"



/// <summary>
/// Default view direction is -z = transform->backward
/// </summary>
class Camera : public Component
{
private: // Members:
	float m_fov;
	float m_aspectRatio;
	float m_nearClip;
	float m_farClip;
	Float4x4 m_projectionMatrix;
	bool m_updateProjectionMatrix;
	bool m_drawFrustum;

public: // Methods:
	Camera();
	~Camera();

	// Setters:
	void SetFov(float fov);
	void SetAspectRatio(float aspectRatio);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);
	void SetDrawFrustum(bool drawFrustum);

	// Getters:
	float GetFov() const;
	float GetAspectRatio() const;
	float GetNearClip() const;
	float GetFarClip() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	void LateUpdate() override;
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_camera_h__