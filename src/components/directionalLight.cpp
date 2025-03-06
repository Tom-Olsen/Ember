#include "directionalLight.h"
#include "shadowCascade.h"
#include "logger.h"



namespace emberEngine
{
	// Constructor/Destructor:
	DirectionalLight::DirectionalLight()
	{
		m_intensity = 1.0f;
		m_color = Float3::white;
		m_shadowType = Lighting::ShadowType::hard;
		m_pActiveCamera = nullptr;
		m_shadowCascadeCount = 4;
		m_distributionFactor = 0.0f;
		for (int i = 0; i < 5; i++)
			m_shadowCascadeSplits[i] = (float)i / 4;
		for (int i = 0; i < 4; i++)
			m_shadowCascades[i] = new ShadowCascade();
		m_drawFrustum = false;
	}
	DirectionalLight::~DirectionalLight()
	{
		for (int i = 0; i < 4; i++)
			delete m_shadowCascades[i];
	}



	// Public methods:
	// Setters:
	void DirectionalLight::SetIntensity(float intensity)
	{
		m_intensity = std::clamp(intensity, 0.0f, 1.0f);
	}
	void DirectionalLight::SetColor(const Float3& color)
	{
		m_color = color;
	}
	void DirectionalLight::SetShadowType(Lighting::ShadowType shadowType)
	{
		m_shadowType = shadowType;
	}
	void DirectionalLight::SetActiveCamera(Camera* pCamera)
	{
		m_pActiveCamera = pCamera;
	}
	void DirectionalLight::SetShadowCascadeCount(int shadowCascadeCount)
	{
		if (m_shadowCascadeCount < 0)
		{
			LOG_WARN("Directional Light Shadow Cascade Count '{}' invalid. Setting it to min value, 1.", shadowCascadeCount);
			shadowCascadeCount = 1;
		}
		if (m_shadowCascadeCount > 4)
		{
			LOG_WARN("Directional Light Shadow Cascade Count '{}' invalid. Setting it to max value, 4.", shadowCascadeCount);
			shadowCascadeCount = 4;
		}
		m_shadowCascadeCount = shadowCascadeCount;
	}
	void DirectionalLight::SetDistributionFactor(float distributionFactor)
	{
		m_distributionFactor = distributionFactor;
	}
	void DirectionalLight::SetDrawFrustum(bool drawFrustum)
	{
		m_drawFrustum = drawFrustum;
	}

	// Getters:
	float DirectionalLight::GetIntensity() const
	{
		return m_intensity;
	}
	Float3 DirectionalLight::GetColor() const
	{
		return m_color;
	}
	Lighting::ShadowType DirectionalLight::GetShadowType() const
	{
		return m_shadowType;
	}
	Camera* DirectionalLight::GetActiveCamera() const
	{
		return m_pActiveCamera;
	}
	int DirectionalLight::GetShadowCascadeCount() const
	{
		return m_shadowCascadeCount;
	}
	float DirectionalLight::GetDistributionFactor() const
	{
		return m_distributionFactor;
	}
	float DirectionalLight::GetShadowCascadeSplit(int index) const
	{
		index = math::Clamp(index, 0, 4);
		return m_shadowCascadeSplits[index];
	}
	Float4x4 DirectionalLight::GetViewMatrix(int shadowCascadeIndex) const
	{
		float sceneHeight = 30.0f; // this is for testing, get height from scene later.
		Float3 direction = GetTransform()->GetDown();
		m_shadowCascades[shadowCascadeIndex]->Update(m_pActiveCamera, direction, m_shadowCascadeSplits[shadowCascadeIndex], m_shadowCascadeSplits[shadowCascadeIndex + 1], sceneHeight);
		return m_shadowCascades[shadowCascadeIndex]->GetViewMatrix();
	}
	Float4x4 DirectionalLight::GetProjectionMatrix(int shadowCascadeIndex)
	{
		float sceneHeight = 30.0f; // this is for testing, get height from scene later.
		Float3 direction = GetTransform()->GetDown();
		m_shadowCascades[shadowCascadeIndex]->Update(m_pActiveCamera, direction, m_shadowCascadeSplits[shadowCascadeIndex], m_shadowCascadeSplits[shadowCascadeIndex + 1], sceneHeight);
		return m_shadowCascades[shadowCascadeIndex]->GetProjectionMatrix();
	}



	// Overrides:
	void DirectionalLight::LateUpdate()
	{
		// Set camera:
		Camera* camera = (m_pActiveCamera == nullptr) ? GetScene()->GetActiveCamera() : m_pActiveCamera;

		// Set splits for shadow cascadess:
		float n = camera->GetNearClip();
		float f = camera->GetFarClip();
		for (int i = 0; i < m_shadowCascadeCount + 1; i++)
		{
			float linear = (float)i / (float)m_shadowCascadeCount;
			float logarithmic = (n * math::Pow(((f + n) / n), ((float)i / (float)m_shadowCascadeCount)) - n) / f;
			m_shadowCascadeSplits[i] = (1.0f - m_distributionFactor) * linear + m_distributionFactor * logarithmic;
		}

		// Update shadow cascades and add directional lights to Lighting system:
		float sceneHeight = 30.0f; // this is for testing, get height from scene later.
		for (int i = 0; i < m_shadowCascadeCount; i++)
		{
			Float3 direction = GetTransform()->GetDown();
			m_shadowCascades[i]->Update(camera, direction, m_shadowCascadeSplits[i], m_shadowCascadeSplits[i + 1], sceneHeight);
			Float4x4 worldToClipMatrix = m_shadowCascades[i]->GetProjectionMatrix() * m_shadowCascades[i]->GetViewMatrix();
			Lighting::AddDirectionalLight(direction, m_intensity, m_color, m_shadowType, worldToClipMatrix);
		}

		// Visualization for debugging:
		if (m_drawFrustum)
		{
			Material* pUnlitMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
			Material* pVertexUnlit = MaterialManager::GetMaterial("vertexColorUnlitMaterial");
			Mesh* pSphere = MeshManager::GetMesh("cubeSphere");
			Mesh* fourLeg = MeshManager::GetMesh("fourLeg");
			ShaderProperties* pShaderProperties = nullptr;

			Float4 colors[4] = { Float4::red, Float4::green, Float4::blue, Float4::yellow };
			for (int i = 0; i < (int)m_shadowCascadeCount; i++)
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