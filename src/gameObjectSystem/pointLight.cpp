#include "pointLight.h"
#include "shadowRenderPass.h"



// Static members:
bool PointLight::s_rotationMatricesInitialized = false;
Float4x4 PointLight::s_rotationMatrices[6];



// Constructor/Destructor:
PointLight::PointLight()
{
	m_intensity = 1.0f;
	m_color = Float3::white;
	m_nearClip = 0.1f;
	m_farClip = 15.0f;
	m_updateProjectionMatrix = true;
	m_drawFrustum = false;

	if (!s_rotationMatricesInitialized)
	{
		s_rotationMatricesInitialized = true;
		s_rotationMatrices[0] = Float4x4::identity;						// look backward
		s_rotationMatrices[1] = Float4x4::RotateY(mathf::PI_2);	// look right
		s_rotationMatrices[2] = Float4x4::RotateY(mathf::PI);	// look forward
		s_rotationMatrices[3] = Float4x4::RotateY(-mathf::PI_2);	// look left
		s_rotationMatrices[4] = Float4x4::RotateX(mathf::PI_2);	// look down
		s_rotationMatrices[5] = Float4x4::RotateX(-mathf::PI_2);	// look up
	}
}
PointLight::~PointLight()
{

}



// Setters:
void PointLight::SetIntensity(const float& intensity)
{
	m_intensity = intensity;
}
void PointLight::SetColor(const Float3& color)
{
	m_color = color;
}
void PointLight::SetNearClip(const float& nearClip)
{
	m_nearClip = nearClip;
	m_updateProjectionMatrix = true;
}
void PointLight::SetFarClip(const float& farClip)
{
	m_farClip = farClip;
	m_updateProjectionMatrix = true;
}
void PointLight::SetDrawFrustum(bool drawFrustum)
{
	m_drawFrustum = drawFrustum;
}



// Getters:
Float3 PointLight::GetPosition() const
{
	return GetTransform()->GetPosition();
}
float PointLight::GetIntensity() const
{
	return m_intensity;
}
Float3 PointLight::GetColor() const
{
	return m_color;
}
Float4 PointLight::GetColorIntensity() const
{
	return Float4(m_color, m_intensity);
}
float PointLight::GetNearClip() const
{
	return m_nearClip;
}
float PointLight::GetFarClip() const
{
	return m_farClip;
}
Float4x4 PointLight::GetViewMatrix(uint32_t faceIndex) const
{
	if (faceIndex >= 6)
		throw std::runtime_error("PointLight::GetViewMatrix(uint32_t) faceIndex out of range!");
	return s_rotationMatrices[faceIndex] * GetTransform()->GetWorldToLocalMatrix();
}
Float4x4 PointLight::GetProjectionMatrix()
{
	if (m_updateProjectionMatrix)
		UpdateProjectionMatrix();
	return m_projectionMatrix;
}



// Private methods:
void PointLight::UpdateProjectionMatrix()
{
	m_updateProjectionMatrix = false;
	constexpr float fovRadians = mathf::PI_2;
	constexpr float aspectRatio = 1.0f;
		m_projectionMatrix = Float4x4::Perspective(fovRadians, aspectRatio, m_nearClip, m_farClip);
}



// Overrides:
void PointLight::LateUpdate()
{
	if (m_drawFrustum)
		for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
			Graphics::DrawFrustum(m_pTransform->GetLocalToWorldMatrix(), GetProjectionMatrix() * s_rotationMatrices[faceIndex], m_color);
}
const std::string PointLight::ToString() const
{
	return "PointLight";
}