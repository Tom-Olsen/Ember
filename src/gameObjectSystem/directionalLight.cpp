#include "directionalLight.h"
#include "shadowCascade.h"



// Constructor/Destructor:
DirectionalLight::DirectionalLight()
{
	m_intensity = 1.0f;
	m_color = Float3::white;
	m_nearClip = 0.01f;
	m_farClip = 15.0f;
	m_viewWidth = 15.0f;
	m_viewHeight = 15.0f;
	m_updateProjectionMatrix = true;
	m_maxShadowDistance = 100.0f;

	// Shadow cascade settings:
	m_pActiveCamera = nullptr;
	m_shadowCascadeCount = ShadowCascadeCount::four;
	m_shadowCascadeSplits[0] = 0.1;
	m_shadowCascadeSplits[1] = 0.3;
	m_shadowCascadeSplits[2] = 0.6;

	// Visualization:
	m_drawFrustum = false;
	m_showShaowCascades = false;
	m_overwriteSceneActiveCamera = false;
	pQuad = nullptr;
}
DirectionalLight::~DirectionalLight()
{

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

// Shadow cascade properties:
void DirectionalLight::SetActiveCamera(Camera* pCamera)
{
	if (!m_overwriteSceneActiveCamera)
		m_pActiveCamera = pCamera;
}
void DirectionalLight::SetActiveCamera(Camera* pCamera, bool overwriteSceneActiveCamera)
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
}
void DirectionalLight::SetMaxShadowDistance(float maxShadowDistance)
{
	m_maxShadowDistance = maxShadowDistance;
}
void DirectionalLight::SetShadowCascadeCount(ShadowCascadeCount shadowCascadeCount)
{
	m_shadowCascadeCount = shadowCascadeCount;
}
void DirectionalLight::SetShadowCascadeSplits(float value01, uint32_t index)
{
	if (index < 3)
		m_shadowCascadeSplits[index] = std::clamp(value01, 0.0f, 1.0f);
}

// Visualization bools:
void DirectionalLight::SetDrawFrustum(bool drawFrustum)
{
	m_drawFrustum = drawFrustum;
}
void DirectionalLight::SetShowShadowCascades(bool showShadowCascades)
{
	m_showShaowCascades = showShadowCascades;
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
	if (m_updateProjectionMatrix)
		UpdateProjectionMatrix();
	return m_projectionMatrix;
}

// Shadow cascade properties:
float DirectionalLight::GetMaxShadowDistance() const
{
	return m_maxShadowDistance;
}
DirectionalLight::ShadowCascadeCount DirectionalLight::GetShadowCascadeCount() const
{
	return m_shadowCascadeCount;
}
const float* const DirectionalLight::GetShadowCascadeSplits() const
{
	return m_shadowCascadeSplits;
}



// Private methods:
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
void DirectionalLight::Start()
{
	SetActiveCamera(m_pScene->GetActiveCamera());
}
void DirectionalLight::LateUpdate()
{
	if (m_drawFrustum)
		Graphics::DrawFrustum(m_pTransform->GetLocalToWorldMatrix(), GetProjectionMatrix(), 0.1f, Float4(m_color,1.0f));

	if (m_showShaowCascades)
	{
		if (pQuad == nullptr)
			pQuad = std::unique_ptr<Mesh>(MeshGenerator::UnitQuad()->Rotate(Float4x4::rot270x));

		// Camera transformation matrices:
		Float4x4 projectionMatrix = m_pActiveCamera->GetProjectionMatrix();
		Float4x4 invProjectionMatrix = projectionMatrix.Inverse();
		Float4x4 localToWorldMatrix = m_pActiveCamera->GetTransform()->GetLocalToWorldMatrix();
		Float4x4 directionWorldToLocalMatrix = localToWorldMatrix.Transpose();

		// Light direction in local space of camera:
		Float3 localDirection = Float3x3(directionWorldToLocalMatrix) * GetDirection();

		// Center points of near/far planes in local space of camera:
		Float4 centerNear = invProjectionMatrix * Float4(0, 0, -1, 1);
		Float4 centerFar = invProjectionMatrix * Float4(0, 0, 1, 1);
		centerNear /= centerNear.w;
		centerFar /= centerFar.w;

		// Center positions of shadow cascade regions in local space of camera:
		Float3 dir = Float3(centerFar - centerNear);
		Float3 center0 = Float3(centerNear) + dir * 0.5f * m_shadowCascadeSplits[0];
		Float3 center1 = Float3(centerNear) + dir * (m_shadowCascadeSplits[0] + 0.5f * (m_shadowCascadeSplits[1] - m_shadowCascadeSplits[0]));
		Float3 center2 = Float3(centerNear) + dir * (m_shadowCascadeSplits[1] + 0.5f * (m_shadowCascadeSplits[2] - m_shadowCascadeSplits[1]));
		Float3 center3 = Float3(centerNear) + dir * (m_shadowCascadeSplits[2] + 0.5f * (1.0f - m_shadowCascadeSplits[2]));

		// Corner points of near/far planes in local space of camera:
		Float4 cornerNear = invProjectionMatrix * Float4(-1, -1, -1, 1);
		Float4 cornerFar = invProjectionMatrix * Float4(-1, -1, 1, 1);
		cornerNear /= cornerNear.w;
		cornerFar /= cornerFar.w;

		// Corner points of shadow cascade regions in local space of camera:
		dir = Float3(cornerFar - cornerNear);
		Float3 corner0 = Float3(cornerNear) + dir * m_shadowCascadeSplits[0];
		Float3 corner1 = Float3(cornerNear) + dir * m_shadowCascadeSplits[1];
		Float3 corner2 = Float3(cornerNear) + dir * m_shadowCascadeSplits[2];
		Float3 corner3 = Float3(cornerFar);

		// Draw calls:
		Material* pUnlitMaterial = MaterialManager::GetMaterial("simpleUnlit");
		Material* pVertexUnlit = MaterialManager::GetMaterial("vertexColorUnlit");
		Mesh* pSphere = MeshManager::GetMesh("cubeSphere");
		Mesh* fourLeg = MeshManager::GetMesh("fourLeg");
		MaterialProperties* pMaterialProperties = nullptr;
		{// Draw shadow cascade center points:
			Float4x4 model0 = localToWorldMatrix * Float4x4::TRS(center0, Float4x4::identity, Float3::one);
			Float4x4 model1 = localToWorldMatrix * Float4x4::TRS(center1, Float4x4::identity, Float3::one);
			Float4x4 model2 = localToWorldMatrix * Float4x4::TRS(center2, Float4x4::identity, Float3::one);
			Float4x4 model3 = localToWorldMatrix * Float4x4::TRS(center3, Float4x4::identity, Float3::one);
			pMaterialProperties = Graphics::DrawMesh(pSphere, pUnlitMaterial, model0, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
			pMaterialProperties = Graphics::DrawMesh(pSphere, pUnlitMaterial, model1, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
			pMaterialProperties = Graphics::DrawMesh(pSphere, pUnlitMaterial, model2, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
			pMaterialProperties = Graphics::DrawMesh(pSphere, pUnlitMaterial, model3, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
		}
		{// Draw shadow cascade areas:
			Float3 scale0 = Float3(2.0f * mathf::Abs(corner0.x - center0.x), 1.0f, 2.0f * mathf::Abs(corner0.z - center0.z));
			Float3 scale1 = Float3(2.0f * mathf::Abs(corner1.x - center1.x), 1.0f, 2.0f * mathf::Abs(corner1.z - center1.z));
			Float3 scale2 = Float3(2.0f * mathf::Abs(corner2.x - center2.x), 1.0f, 2.0f * mathf::Abs(corner2.z - center2.z));
			Float3 scale3 = Float3(2.0f * mathf::Abs(corner3.x - center3.x), 1.0f, 2.0f * mathf::Abs(corner3.z - center3.z));
			Float4x4 model0 = localToWorldMatrix * Float4x4::TRS(center0, Float4x4::identity, scale0);
			Float4x4 model1 = localToWorldMatrix * Float4x4::TRS(center1, Float4x4::identity, scale1);
			Float4x4 model2 = localToWorldMatrix * Float4x4::TRS(center2, Float4x4::identity, scale2);
			Float4x4 model3 = localToWorldMatrix * Float4x4::TRS(center3, Float4x4::identity, scale3);
			pMaterialProperties = Graphics::DrawMesh(pQuad.get(), pUnlitMaterial, model0, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 0.0f, 0.0f, 1.0f));
			pMaterialProperties = Graphics::DrawMesh(pQuad.get(), pUnlitMaterial, model1, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.7f, 0.0f, 0.0f, 1.0f));
			pMaterialProperties = Graphics::DrawMesh(pQuad.get(), pUnlitMaterial, model2, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.4f, 0.0f, 0.0f, 1.0f));
			pMaterialProperties = Graphics::DrawMesh(pQuad.get(), pUnlitMaterial, model3, false, false);
			pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.1f, 0.0f, 0.0f, 1.0f));
		}
		{
			ShadowCascade shadowCascade0(m_pActiveCamera, GetDirection(), 0, m_shadowCascadeSplits[0], 100.0f);
			ShadowCascade shadowCascade1(m_pActiveCamera, GetDirection(), m_shadowCascadeSplits[0], m_shadowCascadeSplits[1], 100.0f);
			ShadowCascade shadowCascade2(m_pActiveCamera, GetDirection(), m_shadowCascadeSplits[1], m_shadowCascadeSplits[2], 100.0f);
			ShadowCascade shadowCascade3(m_pActiveCamera, GetDirection(), m_shadowCascadeSplits[2], 1, 100.0f);
			Float4x4 model0 = shadowCascade0.GetLightLocalToWorldMatrix();
			Float4x4 model1 = shadowCascade1.GetLightLocalToWorldMatrix();
			Float4x4 model2 = shadowCascade2.GetLightLocalToWorldMatrix();
			Float4x4 model3 = shadowCascade3.GetLightLocalToWorldMatrix();
			
			// Draw light fourLeg:
			Graphics::DrawMesh(fourLeg, pVertexUnlit, model0, false, false);
			Graphics::DrawMesh(fourLeg, pVertexUnlit, model1, false, false);
			Graphics::DrawMesh(fourLeg, pVertexUnlit, model2, false, false);
			Graphics::DrawMesh(fourLeg, pVertexUnlit, model3, false, false);
			
			// Draw light frustums:
			Graphics::DrawFrustum(model0, shadowCascade0.GetProjectionMatrix());
			Graphics::DrawFrustum(model1, shadowCascade1.GetProjectionMatrix());
			Graphics::DrawFrustum(model2, shadowCascade2.GetProjectionMatrix());
			Graphics::DrawFrustum(model3, shadowCascade3.GetProjectionMatrix());
		}
	}
}
const std::string DirectionalLight::ToString() const
{
	return "DirectionalLight";
}