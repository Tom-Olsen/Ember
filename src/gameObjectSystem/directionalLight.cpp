#include "directionalLight.h"
#include "shadowCascade.h"
using namespace emberEngine;



namespace emberEngine
{
	// Constructor/Destructor:
	DirectionalLight::DirectionalLight()
	{
		m_intensity = 1.0f;
		m_color = Float3::white;
		m_shadowType = ShadowType::hard;

		// Shadow cascade settings:
		for (uint32_t i = 0; i < 4; i++)
			m_shadowCascades[i] = new ShadowCascade();
		m_pActiveCamera = nullptr;
		m_distributionFactor = 0.5f;
		m_shadowCascadeCount = ShadowCascadeCount::four;

		// Visualization:
		m_drawFrustum = false;
		m_overwriteSceneActiveCamera = false;
	}
	DirectionalLight::~DirectionalLight()
	{

	}



	// Public methods:
	void DirectionalLight::UpdateShadowCascades(float sceneHeight)
	{
		for (uint32_t i = 0; i < (uint32_t)m_shadowCascadeCount; i++)
			m_shadowCascades[i]->Update(m_pActiveCamera, GetDirection(), m_shadowCascadeSplits[i], m_shadowCascadeSplits[i + 1], sceneHeight);
	}



	// Setters:
	// Light properties:
	void DirectionalLight::SetIntensity(float intensity)
	{
		m_intensity = std::clamp(intensity, 0.0f, 1.0f);
	}
	void DirectionalLight::SetColor(const Float3& color)
	{
		m_color = color;
	}
	void DirectionalLight::SetShadowType(ShadowType shadowType)
	{
		m_shadowType = shadowType;
	}

	// Shadow cascade properties:
	void DirectionalLight::SetActiveCamera(Camera* pCamera)
	{
		if (!m_overwriteSceneActiveCamera)
		{
			m_pActiveCamera = pCamera;
			m_distributionFactor = -m_distributionFactor;
			SetDistributionFactor(-m_distributionFactor);
		}
	}
	void DirectionalLight::OverwriteActiveCamera(Camera* pCamera, bool overwriteSceneActiveCamera)
	{
		if (pCamera == nullptr || !overwriteSceneActiveCamera)
		{
			m_pActiveCamera = m_pScene->GetActiveCamera();
			m_overwriteSceneActiveCamera = false;
		}
		else
		{
			m_overwriteSceneActiveCamera = overwriteSceneActiveCamera;
			m_pActiveCamera = pCamera;
		}
		m_distributionFactor = -m_distributionFactor;
		SetDistributionFactor(-m_distributionFactor);
	}
	void DirectionalLight::SetShadowCascadeCount(ShadowCascadeCount shadowCascadeCount)
	{
		m_shadowCascadeCount = shadowCascadeCount;
		// Force recalculation of shadow cascade splits:
		m_distributionFactor = -m_distributionFactor;
		SetDistributionFactor(-m_distributionFactor);
	}
	void DirectionalLight::SetDistributionFactor(float value01)
	{
		if (m_distributionFactor != value01)
		{
			m_distributionFactor = value01;
			if (m_pActiveCamera != nullptr)
			{
				float n = m_pActiveCamera->GetNearClip();
				float f = m_pActiveCamera->GetFarClip();
				uint32_t count = (uint32_t)m_shadowCascadeCount;
				for (uint32_t i = 0; i < count + 1; i++)
				{
					float linear = (float)i / (float)count;
					float logarithmic = (n * mathf::Pow(((f + n) / n), (i / (float)count)) - n) / f;
					m_shadowCascadeSplits[i] = (1.0f - m_distributionFactor) * linear + m_distributionFactor * logarithmic;
				}
			}
		}
	}

	// Visualization bools:
	void DirectionalLight::SetDrawFrustum(bool drawFrustum)
	{
		m_drawFrustum = drawFrustum;
	}

	// Getters:
	// Light properties:
	///<summary>
	/// Direction from light to target (must be flipped in shaders).
	/// </summary>
	Float3 DirectionalLight::GetDirection() const
	{
		return GetTransform()->GetDown();
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
	ShadowType DirectionalLight::GetShadowType() const
	{
		return m_shadowType;
	}
	Float4x4 DirectionalLight::GetViewMatrix(uint32_t shadowCascadeIndex) const
	{
		return m_shadowCascades[shadowCascadeIndex]->GetViewMatrix();
	}
	Float4x4 DirectionalLight::GetProjectionMatrix(uint32_t shadowCascadeIndex)
	{
		return m_shadowCascades[shadowCascadeIndex]->GetProjectionMatrix();
	}

	// Shadow cascade properties:
	DirectionalLight::ShadowCascadeCount DirectionalLight::GetShadowCascadeCount() const
	{
		return m_shadowCascadeCount;
	}
	float DirectionalLight::GetDistributionFactor() const
	{
		return m_distributionFactor;
	}
	float DirectionalLight::GetShadowCascadeSplit(uint32_t index) const
	{
		return m_shadowCascadeSplits[index];
	}


	// Overrides:
	void DirectionalLight::Start()
	{
		SetActiveCamera(m_pScene->GetActiveCamera());
	}
	void DirectionalLight::LateUpdate()
	{
		if (m_drawFrustum)
		{
			Material* pUnlitMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
			Material* pVertexUnlit = MaterialManager::GetMaterial("vertexColorUnlitMaterial");
			Mesh* pSphere = MeshManager::GetMesh("cubeSphere");
			Mesh* fourLeg = MeshManager::GetMesh("fourLeg");
			ShaderProperties* pShaderProperties = nullptr;

			Float4 colors[4] = { Float4::red, Float4::green, Float4::blue, Float4::yellow };
			for (uint32_t i = 0; i < (int)m_shadowCascadeCount; i++)
			{
				Float4x4 localToWorldMatrix = m_shadowCascades[i]->GetViewMatrix().Inverse();
				Graphics::DrawMesh(fourLeg, pVertexUnlit, localToWorldMatrix, false, false);
				Graphics::DrawFrustum(localToWorldMatrix, m_shadowCascades[i]->GetProjectionMatrix(), 0.1f, colors[i]);
			}
		}
	}
	const std::string DirectionalLight::ToString() const
	{
		return "DirectionalLight";
	}
}