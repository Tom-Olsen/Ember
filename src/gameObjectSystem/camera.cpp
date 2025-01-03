#include "camera.h"



// Constructor/Destructor:
Camera::Camera()
{
	m_fov = mathf::DEG2RAD * 60.0f;
	m_aspectRatio = 16.0f / 9.0f;	// 1920x1080
	m_nearClip = 0.1f;
	m_farClip = 1000.0f;
	m_updateProjectionMatrix = true;
	m_drawFrustum = false;
}
Camera::~Camera()
{

}



// Setters:
void Camera::SetFov(float fov)
{
	m_fov = fov;
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
void Camera::SetDrawFrustum(bool drawFrustum)
{
	m_drawFrustum = drawFrustum;
}



// Getters:
float Camera::GetFov() const
{
	return m_fov;
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
	m_projectionMatrix = Float4x4::Perspective(m_fov, m_aspectRatio, m_nearClip, m_farClip);
}



// Overrides:
void Camera::LateUpdate()
{
	if (m_drawFrustum)
		Graphics::DrawFrustum(m_pTransform->GetLocalToWorldMatrix(), GetProjectionMatrix());
}
const std::string Camera::ToString() const
{
	return "Camera";
}