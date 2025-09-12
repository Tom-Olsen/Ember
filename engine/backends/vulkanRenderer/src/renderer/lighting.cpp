#include "lighting.h"
#include <math.h>



namespace vulkanRendererBackend
{
	// Static private members:
	bool Lighting::s_isInitialized = false;
	uint32_t Lighting::s_directionalLightsCount;
	uint32_t Lighting::s_positionalLightsCount;
	uint32_t Lighting::s_maxDirectionalLights;
	uint32_t Lighting::s_maxPositionalLights;
	uint32_t Lighting::s_shadowMapResolution;
	std::vector<Lighting::DirectionalLight> Lighting::s_directionalLights;
	std::vector<Lighting::PositionalLight> Lighting::s_positionalLights;
	Float4x4 Lighting::s_pointLightRotationMatrices[6];


	// Initialization/Cleanup:
	void Lighting::Init(uint32_t maxDirectionalLights, uint32_t maxPositionalLights, uint32_t shadowMapResolution)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_directionalLightsCount = 0;
		s_positionalLightsCount = 0;

        s_maxDirectionalLights = maxDirectionalLights;
        s_maxPositionalLights = maxPositionalLights;
        s_shadowMapResolution = shadowMapResolution;

        s_directionalLights.resize(s_maxDirectionalLights);
        s_positionalLights.resize(s_maxPositionalLights);

		s_pointLightRotationMatrices[0] = Float4x4::identity;
		s_pointLightRotationMatrices[1] = Float4x4::RotateY( pi2);
		s_pointLightRotationMatrices[2] = Float4x4::RotateY( pi );	
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(-pi2);
		s_pointLightRotationMatrices[4] = Float4x4::RotateX( pi2);
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(-pi2);
	}
	void Lighting::Clear()
	{

	}



	// Public methods:
	// Adders:
	void Lighting::AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, ShadowType shadowType, const Float4x4& worldToClipMatrix)
	{
		if (s_directionalLightsCount == s_maxDirectionalLights)
			return;

		s_directionalLights[s_directionalLightsCount].direction = direction;
		s_directionalLights[s_directionalLightsCount].intensity = intensity;
		s_directionalLights[s_directionalLightsCount].color = color;
		s_directionalLights[s_directionalLightsCount].shadowType = shadowType;
		s_directionalLights[s_directionalLightsCount].worldToClipMatrix = worldToClipMatrix;

		s_directionalLightsCount++;
	}
	void Lighting::AddPositionalLight(const Float3& position, float intensity, const Float3& color, ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix)
	{
		if (s_positionalLightsCount == s_maxPositionalLights)
			return;

		s_positionalLights[s_positionalLightsCount].position = position;
		s_positionalLights[s_positionalLightsCount].intensity = intensity;
		s_positionalLights[s_positionalLightsCount].color = color;
		s_positionalLights[s_positionalLightsCount].shadowType = shadowType;
		s_positionalLights[s_positionalLightsCount].blendStart = blendStart;
		s_positionalLights[s_positionalLightsCount].blendEnd = blendEnd;
		s_positionalLights[s_positionalLightsCount].worldToClipMatrix = worldToClipMatrix;

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
	std::vector<Lighting::DirectionalLight>& Lighting::GetDirectionalLights()
	{
		return s_directionalLights;
	}
	std::vector<Lighting::PositionalLight>& Lighting::GetPositionalLights()
	{
		return s_positionalLights;
	}
	Float4x4 Lighting::GetPointLightRotationMatrix(uint32_t faceIndex)
	{
		int index = std::clamp((int)faceIndex, 0, 5);
		return s_pointLightRotationMatrices[faceIndex];
	}



	// Debugging:
	std::string Lighting::DirectionalLight::ToString()
	{
		std::string output = "";
		output += "direction: " + direction.ToString() + "\n";
		output += "intensity: " + std::to_string(intensity) + "\n";
		output += "color: " + color.ToString() + "\n";
		if (shadowType == Lighting::ShadowType::hard)
			output += "shadowType: hard\n";
		else if (shadowType == Lighting::ShadowType::soft)
			output += "shadowType: soft\n";
		output += "worldToClipMatrix: " + worldToClipMatrix.ToString() + "\n";
		return output;
	}
	std::string Lighting::PositionalLight::ToString()
	{
		std::string output = "";
		output += "position: " + position.ToString() + "\n";
		output += "intensity: " + std::to_string(intensity) + "\n";
		output += "color: " + color.ToString() + "\n";
		if (shadowType == Lighting::ShadowType::hard)
			output += "shadowType: hard\n";
		else if (shadowType == Lighting::ShadowType::soft)
			output += "shadowType: soft\n";
		output += "blendStart: " + std::to_string(blendStart) + "\n";
		output += "blendEnd: " + std::to_string(blendEnd) + "\n";
		output += "worldToClipMatrix: " + worldToClipMatrix.ToString() + "\n";
		return output;
	}
}