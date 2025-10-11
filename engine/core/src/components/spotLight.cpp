#include "spotLight.h"



namespace emberEngine
{
	// Constructor/Destructor:
	SpotLight::SpotLight()
	{
		m_intensity = 1.0f;
		m_color = Float3::white;
		m_shadowType = emberCommon::ShadowType::hard;
		m_fov = math::deg2rad * 45.0f;
		m_nearClip = 0.1f;
		m_farClip = 15.0f;
		m_blendStart = 0.8f;
		m_blendEnd = 1.0f;
		m_updateProjectionMatrix = true;
		m_drawFrustum = false;
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
	void SpotLight::SetShadowType(ShadowType shadowType)
	{
		m_shadowType = shadowType;
	}
	void SpotLight::SetFov(const float& fov)
	{
		m_fov = fov;
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
		m_blendStart = math::Clamp(blendStart, 0.0f, 1.0f);
	}
	void SpotLight::SetBlendEnd(const float& blendEnd)
	{
		m_blendEnd = math::Clamp(blendEnd, 0.0f, 1.0f);
	}
	void SpotLight::SetDrawFrustum(bool drawFrustum)
	{
		m_drawFrustum = drawFrustum;
	}



	// Getters:
	float SpotLight::GetIntensity() const
	{
		return m_intensity;
	}
	Float3 SpotLight::GetColor() const
	{
		return m_color;
	}
	ShadowType SpotLight::GetShadowType() const
	{
		return m_shadowType;
	}
	float SpotLight::GetFov() const
	{
		return m_fov;
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
	Float4x4 SpotLight::GetViewMatrix() const
	{
		return GetTransform()->GetWorldToLocalMatrix();
	}
	Float4x4 SpotLight::GetProjectionMatrix()
	{
		if (m_updateProjectionMatrix)
			UpdateProjectionMatrix();
		return m_projectionMatrix;
	}



	// Private methods:
	void SpotLight::UpdateProjectionMatrix()
	{
		m_updateProjectionMatrix = false;
		constexpr float aspectRatio = 1.0f;
		m_projectionMatrix = Float4x4::Perspective(m_fov, aspectRatio, m_nearClip, m_farClip);
	}



	// Overrides:
	void SpotLight::LateUpdate()
	{
		Float4x4 worldToClipMatrix = GetProjectionMatrix() * GetViewMatrix();
		Renderer::AddPositionalLight(GetTransform()->GetPosition(), m_intensity, m_color, m_shadowType, m_blendStart, m_blendEnd, worldToClipMatrix);

		if (m_drawFrustum)
			Renderer::DrawFrustum(GetTransform()->GetLocalToWorldMatrix(), GetProjectionMatrix(), 0.1f, Float4(m_color, 1.0f));
	}
	const std::string SpotLight::ToString() const
	{
		return "SpotLight";
	}
}