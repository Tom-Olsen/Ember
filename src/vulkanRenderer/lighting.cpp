#include "lighting.h"



namespace emberEngine
{
	// Static private members:
	bool Lighting::s_isInitialized = false;
	uint32_t Lighting::s_directionalLightsCount;
	uint32_t Lighting::s_pointLightsCount;
	uint32_t Lighting::s_spotLightsCount;
	std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights> Lighting::s_directionalLights;
	std::array<Lighting::PointLight, Lighting::maxPointLights> Lighting::s_pointLights;
	std::array<Lighting::SpotLight, Lighting::maxSpotLights> Lighting::s_spotLights;
	Float4x4 Lighting::s_pointLightRotationMatrices[6];


	// Initialization/Cleanup:
	void Lighting::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_directionalLightsCount = 0;
		s_pointLightsCount = 0;
		s_spotLightsCount = 0;

		s_pointLightRotationMatrices[0] = Float4x4::identity;
		s_pointLightRotationMatrices[1] = Float4x4::RotateY(math::pi2);
		s_pointLightRotationMatrices[2] = Float4x4::RotateY(math::pi);	
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(-math::pi2);
		s_pointLightRotationMatrices[4] = Float4x4::RotateX(math::pi2);
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(-math::pi2);
	}
	void Lighting::Clear()
	{

	}



	// Setters/Adders:
	void Lighting::SetActiveCamera()
	{

	}
	// Public methods:
	void Lighting::AddPointLight
	(float intensity, const Float3& color, ShadowType shadowType, Float3 position,
	 float nearClip, float farClip, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		if (s_pointLightsCount == maxPointLights)
			return;

		s_pointLights[s_pointLightsCount].intensity = intensity;
		s_pointLights[s_pointLightsCount].color = color;
		s_pointLights[s_pointLightsCount].shadowType = shadowType;
		s_pointLights[s_pointLightsCount].position = position;

		s_pointLights[s_pointLightsCount].nearClip = nearClip;
		s_pointLights[s_pointLightsCount].farClip = farClip;
		for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
			s_pointLights[s_pointLightsCount].viewMatrix[faceIndex] = s_pointLightRotationMatrices[faceIndex] * viewMatrix;
		s_pointLights[s_pointLightsCount].projectionMatrix = projectionMatrix;

		s_pointLightsCount++;
	}
	void Lighting::AddSpotLight
	(float intensity, const Float3& color, ShadowType shadowType, Float3 position,
	 float fov, float nearClip, float farClip, float blendStart, float blendEnd,
	 const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		if (s_spotLightsCount == maxSpotLights)
			return;

		s_spotLights[s_spotLightsCount].intensity = intensity;
		s_spotLights[s_spotLightsCount].color = color;
		s_spotLights[s_spotLightsCount].shadowType = shadowType;
		s_spotLights[s_spotLightsCount].position = position;

		s_spotLights[s_spotLightsCount].fov = fov;
		s_spotLights[s_spotLightsCount].nearClip = nearClip;
		s_spotLights[s_spotLightsCount].farClip = farClip;
		s_spotLights[s_spotLightsCount].blendStart = blendStart;
		s_spotLights[s_spotLightsCount].blendEnd = blendEnd;
		s_spotLights[s_spotLightsCount].viewMatrix = viewMatrix;
		s_spotLights[s_spotLightsCount].projectionMatrix = projectionMatrix;

		s_spotLightsCount++;
	}
	void Lighting::ResetLights()
	{
		s_directionalLightsCount = 0;
		s_pointLightsCount = 0;
		s_spotLightsCount = 0;
	}



	// Getters:
	uint32_t Lighting::GetDirectionalLightsCount()
	{
		return s_directionalLightsCount;
	}
	uint32_t Lighting::GetPointLightsCount()
	{
		return s_pointLightsCount;
	}
	uint32_t Lighting::GetSpotLightsCount()
	{
		return s_spotLightsCount;
	}
	std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& Lighting::GetDirectionalLights()
	{
		return s_directionalLights;
	}
	std::array<Lighting::PointLight, Lighting::maxPointLights>& Lighting::GetPointLights()
	{
		return s_pointLights;
	}
	std::array<Lighting::SpotLight, Lighting::maxSpotLights>& Lighting::GetSpotLights()
	{
		return s_spotLights;
	}
	Float4x4 Lighting::GetPointLightRotationMatrix(uint32_t faceIndex)
	{
		int index = math::Clamp((int)faceIndex, 0, 5);
		return s_pointLightRotationMatrices[faceIndex];
	}
}