#include "lighting.h"



namespace emberEngine
{
	// Static private members:
	bool Lighting::s_isInitialized = false;
	uint32_t Lighting::s_directionalLightsCount;
	uint32_t Lighting::s_positionalLightsCount;
	std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights> Lighting::s_directionalLights;
	std::array<Lighting::PositionalLight, Lighting::maxPositionalLights> Lighting::s_positionalLights;
	Float4x4 Lighting::s_pointLightRotationMatrices[6];


	// Initialization/Cleanup:
	void Lighting::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_directionalLightsCount = 0;
		s_positionalLightsCount = 0;

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
	void Lighting::AddPositionalLight
	(Float3 position, float intensity, const Float3& color, ShadowType shadowType,
	 float fov, float nearClip, float farClip, float blendStart, float blendEnd,
	 const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		if (s_positionalLightsCount == maxPositionalLights)
			return;

		s_positionalLights[s_positionalLightsCount].position = position;
		s_positionalLights[s_positionalLightsCount].intensity = intensity;
		s_positionalLights[s_positionalLightsCount].color = color;
		s_positionalLights[s_positionalLightsCount].shadowType = shadowType;

		s_positionalLights[s_positionalLightsCount].fov = fov;
		s_positionalLights[s_positionalLightsCount].nearClip = nearClip;
		s_positionalLights[s_positionalLightsCount].farClip = farClip;
		s_positionalLights[s_positionalLightsCount].blendStart = blendStart;
		s_positionalLights[s_positionalLightsCount].blendEnd = blendEnd;
		s_positionalLights[s_positionalLightsCount].viewMatrix = viewMatrix;
		s_positionalLights[s_positionalLightsCount].projectionMatrix = projectionMatrix;

		s_positionalLightsCount++;
	}
	void Lighting::ResetLights()
	{
		s_directionalLightsCount = 0;
		s_positionalLightsCount = 0;
	}



	// Getters:
	uint32_t Lighting::GetDirectionalLightsCount()
	{
		return s_directionalLightsCount;
	}
	uint32_t Lighting::GetPositionalLightsCount()
	{
		return s_positionalLightsCount;
	}
	std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& Lighting::GetDirectionalLights()
	{
		return s_directionalLights;
	}
	std::array<Lighting::PositionalLight, Lighting::maxPositionalLights>& Lighting::GetPositionalLights()
	{
		return s_positionalLights;
	}
	Float4x4 Lighting::GetPointLightRotationMatrix(uint32_t faceIndex)
	{
		int index = math::Clamp((int)faceIndex, 0, 5);
		return s_pointLightRotationMatrices[faceIndex];
	}
}