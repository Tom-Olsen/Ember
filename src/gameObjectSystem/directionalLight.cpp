#include "directionalLight.h"



// Constructor/Destructor:
DirectionalLight::DirectionalLight()
{
	m_intensity = 1.0f;
	m_color = Float3::one;
	m_nearClip = 0.01f;
	m_farClip = 15.0f;
	m_viewWidth = 15.0f;
	m_viewHeight = 15.0f;
	m_updateProjectionMatrix = true;
}
DirectionalLight::~DirectionalLight()
{

}



// Setters:
void DirectionalLight::SetIntensity(float intensity)
{
	m_intensity = std::clamp(intensity, 0.0f, 1.0f);
}
void DirectionalLight::SetColor(const Float3& color)
{
	m_color = color;
}
void DirectionalLight::SetNearClip(float nearClip)
{
	m_nearClip = nearClip;
	m_updateProjectionMatrix = true;
}
void DirectionalLight::SetFarClip(float farClip)
{
	m_farClip = farClip;
	m_updateProjectionMatrix = true;
}
void DirectionalLight::SetViewWidth(float viewWidth)
{
	m_viewWidth = viewWidth;
	m_updateProjectionMatrix = true;
}
void DirectionalLight::SetViewHeight(float viewHeight)
{
	m_viewHeight = viewHeight;
	m_updateProjectionMatrix = true;
}



// Getters:
Float3 DirectionalLight::GetDirection() const
{
	return GetTransform()->GetForward();
}
float DirectionalLight::GetIntensity() const
{
	return m_intensity;
}
Float3 DirectionalLight::GetColor() const
{
	return m_color;
}
Float4 DirectionalLight::GetColorIntensity() const
{
	return Float4(m_color, m_intensity);
}
float DirectionalLight::GetNearClip() const
{
	return m_nearClip;
}
float DirectionalLight::GetFarClip() const
{
	return m_farClip;
}
float DirectionalLight::GetViewWidth() const
{
	return m_viewWidth;
}
float DirectionalLight::GetViewHeight() const
{
	return m_viewHeight;
}
Float4x4 DirectionalLight::GetViewMatrix() const
{
	return GetTransform()->GetWorldToLocalMatrix();
}
Float4x4 DirectionalLight::GetProjectionMatrix()
{
	if (m_updateProjectionMatrix && isActive && GetGameObject()->isActive)
		UpdateProjectionMatrix();
	return m_projectionMatrix;
}



// Private:
void DirectionalLight::UpdateProjectionMatrix()
{
	m_updateProjectionMatrix = false;
	float left = -m_viewWidth / 2.0f;
	float right = m_viewWidth / 2.0f;
	float bottom = -m_viewHeight / 2.0f;
	float top = m_viewHeight / 2.0f;
	m_projectionMatrix = Float4x4::Orthographic(left, right, bottom, top, m_nearClip, m_farClip);
}



// Overrides:
const std::string DirectionalLight::ToString() const
{
	return "DirectionalLight";
}