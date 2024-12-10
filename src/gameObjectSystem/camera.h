#ifndef __INCLUDE_GUARD_camera_h__
#define __INCLUDE_GUARD_camera_h__
#include "emberEngine.h"



/// <summary>
/// Default view direction is -z = transform->backward
/// </summary>
class Camera : public Component
{
private: // Members:
	float m_fovRadians;
	float m_aspectRatio;
	float m_nearClip;
	float m_farClip;
	Float4x4 m_projectionMatrix;
	bool m_updateProjectionMatrix;

public: // Methods:
	Camera();
	~Camera();

	// Setters:
	void SetFovDegrees(float fovDegrees);
	void SetFovRadians(float fovRadians);
	void SetAspectRatio(float aspectRatio);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);

	// Getters:
	float GetFovDegrees() const;
	float GetFovRadians() const;
	float GetAspectRatio() const;
	float GetNearClip() const;
	float GetFarClip() const;
	Float4x4 GetViewMatrix() const;
	Float4x4 GetProjectionMatrix();

	// Overrides:
	const std::string ToString() const override;

private: // Methods:
	void UpdateProjectionMatrix();
};



#endif // __INCLUDE_GUARD_camera_h__