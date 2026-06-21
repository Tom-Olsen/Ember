#include "directionalLight.h"
#include "camera.h"
#include "shadowCascade.h"



namespace emberEngine
{
	// Constructor/Destructor:
	DirectionalLight::DirectionalLight()
	{
		m_intensity = 1.0f;
		m_color = Float3::white;
		m_shadowType = ShadowType::hard;
		m_pActiveCamera = nullptr;
		m_shadowCascadeCount = 4;
		m_distributionFactor = 0.0f;
		for (int i = 0; i < 5; i++)
			m_shadowCascadeSplits[i] = (float)i / 4;
		for (int i = 0; i < 4; i++)
		{
			m_shadowCascades[i] = new ShadowCascade();
			m_shadowCascadeMatricesValid[i] = false;
		}
		m_shadowCascadeSplitsValid = false;
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
	void DirectionalLight::SetShadowType(ShadowType shadowType)
	{
		m_shadowType = shadowType;
	}
	void DirectionalLight::SetActiveCamera(Camera* pCamera)
	{
		m_pActiveCamera = pCamera;
		InvalidateShadowCascades();
	}
	void DirectionalLight::SetShadowCascadeCount(int shadowCascadeCount)
	{
		if (shadowCascadeCount < 0)
		{
			LOG_WARN("Directional Light Shadow Cascade Count '{}' invalid. Setting it to min value, 1.", shadowCascadeCount);
			shadowCascadeCount = 1;
		}
		if (shadowCascadeCount > 4)
		{
			LOG_WARN("Directional Light Shadow Cascade Count '{}' invalid. Setting it to max value, 4.", shadowCascadeCount);
			shadowCascadeCount = 4;
		}
		m_shadowCascadeCount = shadowCascadeCount;
		InvalidateShadowCascades();
	}
	void DirectionalLight::SetDistributionFactor(float distributionFactor)
	{
		m_distributionFactor = distributionFactor;
		InvalidateShadowCascades();
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
	ShadowType DirectionalLight::GetShadowType() const
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
	Float4x4 DirectionalLight::GetViewMatrix(int shadowCascadeIndex)
	{
        GrabFallbackCamera();
		Float4x4 cameraLocalToWorldMatrix = m_pActiveCamera->GetTransform()->GetLocalToWorldMatrix();
		Float4x4 cameraProjectionMatrix = m_pActiveCamera->GetProjectionMatrix();
		Float3 direction = GetTransform()->GetDown();
		UpdateShadowCascadeSplits();
		UpdateShadowCascade(shadowCascadeIndex, cameraLocalToWorldMatrix, cameraProjectionMatrix, direction);
		return m_shadowCascades[shadowCascadeIndex]->GetViewMatrix();
	}
	Float4x4 DirectionalLight::GetProjectionMatrix(int shadowCascadeIndex)
	{
        GrabFallbackCamera();
		Float4x4 cameraLocalToWorldMatrix = m_pActiveCamera->GetTransform()->GetLocalToWorldMatrix();
		Float4x4 cameraProjectionMatrix = m_pActiveCamera->GetProjectionMatrix();
		Float3 direction = GetTransform()->GetDown();
		UpdateShadowCascadeSplits();
		UpdateShadowCascade(shadowCascadeIndex, cameraLocalToWorldMatrix, cameraProjectionMatrix, direction);
		return m_shadowCascades[shadowCascadeIndex]->GetProjectionMatrix();
	}



	// Overrides:
	void DirectionalLight::EarlyUpdate()
	{
		InvalidateShadowCascades();
	}
	void DirectionalLight::LateUpdate()
	{
		// Set camera:
		// Update shadow cascades and add directional lights to renderer:
        GrabFallbackCamera();
		Float3 direction = GetTransform()->GetDown();
		UpdateShadowCascadeSplits();
		Float4x4 cameraLocalToWorldMatrix = m_pActiveCamera->GetTransform()->GetLocalToWorldMatrix();
		Float4x4 cameraProjectionMatrix = m_pActiveCamera->GetProjectionMatrix();
		for (int i = 0; i < m_shadowCascadeCount; i++)
		{
			UpdateShadowCascade(i, cameraLocalToWorldMatrix, cameraProjectionMatrix, direction);
			Float4x4 worldToClipMatrix = m_shadowCascades[i]->GetProjectionMatrix() * m_shadowCascades[i]->GetViewMatrix();
			Renderer::AddDirectionalLight(direction, m_intensity, m_color, m_shadowType, worldToClipMatrix);
		}

		// Visualization for debugging:
		if (m_drawFrustum)
		{
			Material pUnlitMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
			Material pVertexUnlit = MaterialManager::GetMaterial("vertexColorUnlitMaterial");
			Mesh& pSphere = MeshManager::GetMesh("cubeSphere");
			Mesh& fourLeg = MeshManager::GetMesh("fourLeg");
			ShaderProperties* pShaderProperties = nullptr;

			Float4 colors[4] = { Float4::red, Float4::green, Float4::blue, Float4::yellow };
			for (int i = 0; i < (int)m_shadowCascadeCount; i++)
			{
				Float4x4 localToWorldMatrix = m_shadowCascades[i]->GetViewMatrix().Inverse();
				Renderer::DrawMesh(fourLeg, pVertexUnlit, localToWorldMatrix, false, false);
				Renderer::DrawFrustum(localToWorldMatrix, m_shadowCascades[i]->GetProjectionMatrix(), 0.1f, colors[i]);
			}
		}
	}



	// Private methods:
    void DirectionalLight::GrabFallbackCamera()
    {
        if (m_pActiveCamera == nullptr)
        {
            m_pActiveCamera = GetEntity().GetScene()->GetActiveCamera();
            InvalidateShadowCascades();
        }
    }
	void DirectionalLight::InvalidateShadowCascades() const
	{
		m_shadowCascadeSplitsValid = false;
		for (int i = 0; i < 4; i++)
			m_shadowCascadeMatricesValid[i] = false;
	}
	void DirectionalLight::UpdateShadowCascadeSplits() const
	{
		if (m_shadowCascadeSplitsValid)
			return;

		float n = m_pActiveCamera->GetNearClip();
		float f = m_pActiveCamera->GetFarClip();
		for (int i = 0; i < m_shadowCascadeCount + 1; i++)
		{
			float linear = (float)i / (float)m_shadowCascadeCount;
			float logarithmic = (n * math::Pow(((f + n) / n), ((float)i / (float)m_shadowCascadeCount)) - n) / f;
			m_shadowCascadeSplits[i] = (1.0f - m_distributionFactor) * linear + m_distributionFactor * logarithmic;
		}
		m_shadowCascadeSplitsValid = true;
	}
	void DirectionalLight::UpdateShadowCascade(int shadowCascadeIndex, const Float4x4& cameraLocalToWorldMatrix, const Float4x4& cameraProjectionMatrix, const Float3& direction) const
	{
		if (m_shadowCascadeMatricesValid[shadowCascadeIndex])
			return;

		float sceneHeight = 30.0f; // this is for testing, get height from scene later.
		m_shadowCascades[shadowCascadeIndex]->Update(cameraLocalToWorldMatrix, cameraProjectionMatrix, direction, m_shadowCascadeSplits[shadowCascadeIndex], m_shadowCascadeSplits[shadowCascadeIndex + 1], sceneHeight);
		m_shadowCascadeMatricesValid[shadowCascadeIndex] = true;
	}
}