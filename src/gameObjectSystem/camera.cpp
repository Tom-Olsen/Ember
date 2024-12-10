#include "camera.h"



// Constructor/Destructor:
Camera::Camera()
{
	m_fovRadians = mathf::ToRadians(60.0f);
	m_aspectRatio = 16.0f / 9.0f;	// 1920x1080
	m_nearClip = 0.1f;
	m_farClip = 100.0f;
	m_updateProjectionMatrix = true;
}
Camera::~Camera()
{

}



// Setters:
void Camera::SetFovDegrees(float fovDegrees)
{
	m_fovRadians = mathf::ToRadians(fovDegrees);
	m_updateProjectionMatrix = true;
}
void Camera::SetFovRadians(float fovRadians)
{
	m_fovRadians = fovRadians;
	m_updateProjectionMatrix = true;
}
void Camera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	m_updateProjectionMatrix = true;
}
void Camera::SetNearClip(float nearClip)
{
	m_nearClip = nearClip;
	m_updateProjectionMatrix = true;
}
void Camera::SetFarClip(float farClip)
{
	m_farClip = farClip;
	m_updateProjectionMatrix = true;
}



// Getters:
float Camera::GetFovDegrees() const
{
	return mathf::ToDegrees(m_fovRadians);
}
float Camera::GetFovRadians() const
{
	return m_fovRadians;
}
float Camera::GetAspectRatio() const
{
	return m_aspectRatio;
}
float Camera::GetNearClip() const
{
	return m_nearClip;
}
float Camera::GetFarClip() const
{
	return m_farClip;
}
Float4x4 Camera::GetViewMatrix() const
{
	return GetTransform()->GetWorldToLocalMatrix();
}
Float4x4 Camera::GetProjectionMatrix()
{
	if (m_updateProjectionMatrix && isActive && GetGameObject()->isActive)
		UpdateProjectionMatrix();
	return m_projectionMatrix;
}



// Private methods:
void Camera::UpdateProjectionMatrix()
{
	m_updateProjectionMatrix = false;
	m_projectionMatrix = Float4x4::Perspective(m_fovRadians, m_aspectRatio, m_nearClip, m_farClip);
}



// Overrides:
const std::string Camera::ToString() const
{
	return "Camera";
}