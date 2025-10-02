#include "pointLight.h"
#include "renderer.h"
#include "vulkanShadowRenderPass.h"



namespace emberEngine
{
	// Constructor/Destructor:
	PointLight::PointLight()
	{
		m_intensity = 1.0f;
		m_color = Float3::white;
		m_shadowType = emberCommon::ShadowType::hard;
		m_nearClip = 0.1f;
		m_farClip = 15.0f;
		m_updateProjectionMatrix = true;
		m_drawFrustum = false;
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
	void PointLight::SetShadowType(emberCommon::ShadowType shadowType)
	{
		m_shadowType = shadowType;
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
	float PointLight::GetIntensity() const
	{
		return m_intensity;
	}
	Float3 PointLight::GetColor() const
	{
		return m_color;
	}
	emberCommon::ShadowType PointLight::GetShadowType() const
	{
		return m_shadowType;
	}
	float PointLight::GetNearClip() const
	{
		return m_nearClip;
	}
	float PointLight::GetFarClip() const
	{
		return m_farClip;
	}
	Float4x4 PointLight::GetViewMatrix() const
	{
		return GetTransform()->GetWorldToLocalMatrix();
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
		constexpr float fov = math::pi2;
		constexpr float aspectRatio = 1.0f;
		m_projectionMatrix = Float4x4::Perspective(fov, aspectRatio, m_nearClip, m_farClip);
	}



	// Overrides:
	void PointLight::LateUpdate()
	{
		// These blend values lead to oversaturation, which willbe clamped back to 1.0 by the saturation functino in the shader, leading to no blending at all:
		constexpr float blendStard = 2.0f;
		constexpr float blendEnd = 3.0f;
		for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
		{
			Float4x4 viewMatrix = Renderer::GetPointLightRotationMatrix(faceIndex) * GetViewMatrix();
			Float4x4 worldToClipMatrix = GetProjectionMatrix() * viewMatrix;
			Renderer::AddPositionalLight(GetTransform()->GetPosition(), m_intensity, m_color, m_shadowType, blendStard, blendEnd, worldToClipMatrix);
		}

		if (m_drawFrustum)
			for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
				Renderer::DrawFrustum(GetTransform()->GetLocalToWorldMatrix(), GetProjectionMatrix() * Renderer::GetPointLightRotationMatrix(faceIndex), 0.1f, Float4(m_color, 1.0f));
	}
	const std::string PointLight::ToString() const
	{
		return "PointLight";
	}
}