#include "spotLight.h"
#include "shadowRenderPass.h"



// Constructor/Destructor:
SpotLight::SpotLight()
{
	m_intensity = 1.0f;
	m_fovRadians = mathf::ToRadians(45.0f);
	m_aspectRatio = (float)ShadowRenderPass::s_shadowMapWidth / (float)ShadowRenderPass::s_shadowMapHeight;
	m_nearClip = 0.1f;
	m_farClip = 15.0f;
	m_blendStart = 0.8f;
	m_blendEnd = 1.0f;
	m_updateProjectionMatrix = true;
}
SpotLight::~SpotLight()
{

}



// Setters:
void SpotLight::SetIntensity(const float& intensity)
{
	m_intensity = intensity;
}
void SpotLight::SetColor(const Float3& color)
{
	m_color = color;
}
void SpotLight::SetFovDegrees(const float& fovDegrees)
{
	m_fovRadians = mathf::ToRadians(fovDegrees);
	m_updateProjectionMatrix = true;
}
void SpotLight::SetFovRadians(const float& fovRadians)
{
	m_fovRadians = fovRadians;
	m_updateProjectionMatrix = true;
}
void SpotLight::SetNearClip(const float& nearClip)
{
	m_nearClip = nearClip;
	m_updateProjectionMatrix = true;
}
void SpotLight::SetFarClip(const float& farClip)
{
	m_farClip = farClip;
	m_updateProjectionMatrix = true;
}
void SpotLight::SetBlendStart(const float& blendStart)
{
	m_blendStart = mathf::Clamp(blendStart, 0.0f, 1.0f);
}
void SpotLight::SetBlendEnd(const float& blendEnd)
{
	m_blendEnd = mathf::Clamp(blendEnd, 0.0f, 1.0f);
}



// Getters:
Float3 SpotLight::GetPosition() const
{
	return GetTransform()->GetPosition();
}
float SpotLight::GetIntensity() const
{
	return m_intensity;
}
Float3 SpotLight::GetColor() const
{
	return m_color;
}
Float4 SpotLight::GetColorIntensity() const
{
	return Float4(m_color, m_intensity);
}
float SpotLight::GetFovDegrees() const
{
	return mathf::ToDegrees(m_fovRadians);
}
float SpotLight::GetFovRadians() const
{
	return m_fovRadians;
}
float SpotLight::GetNearClip() const
{
	return m_nearClip;
}
float SpotLight::GetFarClip() const
{
	return m_farClip;
}
float SpotLight::GetBlendStart() const
{
	return m_blendStart;
}
float SpotLight::GetBlendEnd() const
{
	return m_blendEnd;
}
Float2 SpotLight::GetBlendStartEnd() const
{
	return Float2(m_blendStart, m_blendEnd);
}
Float4x4 SpotLight::GetViewMatrix() const
{
	return GetTransform()->GetWorldToLocalMatrix();
}
Float4x4 SpotLight::GetProjectionMatrix()
{
	if (m_updateProjectionMatrix && isActive && GetGameObject()->isActive)
		UpdateProjectionMatrix();
	return m_projectionMatrix;
}



// Private:
void SpotLight::UpdateProjectionMatrix()
{
	m_updateProjectionMatrix = false;
	m_projectionMatrix = Float4x4::Perspective(m_fovRadians, m_aspectRatio, m_nearClip, m_farClip);
}



// Overrides:
const std::string SpotLight::ToString() const
{
	return "SpotLight";
}