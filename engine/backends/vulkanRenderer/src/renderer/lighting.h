#ifndef __INCLUDE_GUARD_vulkanRendererBackend_lighting_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_lighting_h__
#include "iMath.h"
#include <string>
#include <vector>



namespace vulkanRendererBackend
{
	class Lighting
	{
	public: // Enums and light structs:
		enum class ShadowType { hard, soft };
		struct DirectionalLight
		{
			iMath::Float3 direction;
			float intensity;
			iMath::Float3 color;
			ShadowType shadowType;
			iMath::Float4x4 worldToClipMatrix;
			std::string ToString();
		};
		struct PositionalLight
		{
			iMath::Float3 position;
			float intensity;
			iMath::Float3 color;
			ShadowType shadowType;
			float blendStart;
			float blendEnd;
			iMath::Float4x4 worldToClipMatrix;
			std::string ToString();
		};

	private: // Members:
		static bool s_isInitialized;
		static uint32_t s_directionalLightsCount;
		static uint32_t s_positionalLightsCount;
		static uint32_t s_maxDirectionalLights;
		static uint32_t s_maxPositionalLights;
		static uint32_t s_shadowMapResolution;
		static std::vector<DirectionalLight> s_directionalLights;
		static std::vector<PositionalLight> s_positionalLights;
		static iMath::Float4x4 s_pointLightRotationMatrices[6];

	public: // Methods:
		static void Init(uint32_t maxDirectionalLights, uint32_t maxPositionalLights, uint32_t shadowMapResolution);
		static void Clear();

		// Setters/Adders:
		static void AddDirectionalLight(const iMath::Float3& direction, float intensity, const iMath::Float3& color, ShadowType shadowType, const iMath::Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const iMath::Float3& position, float intensity, const iMath::Float3& color, ShadowType shadowType, float blendStart, float blendEnd, const iMath::Float4x4& worldToClipMatrix);

		// Getters:
		static uint32_t GetDirectionalLightsCount();
		static uint32_t GetPositionalLightsCount();
		static std::vector<DirectionalLight>& GetDirectionalLights();
		static std::vector<PositionalLight>& GetPositionalLights();
		static iMath::Float4x4 GetPointLightRotationMatrix(uint32_t faceIndex);

		// Management:
		static void ResetLights();

	private: // Methods:
		// Delete all constructors:
		Lighting() = delete;
		Lighting(const Lighting&) = delete;
		Lighting& operator=(const Lighting&) = delete;
		~Lighting() = delete;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_lighting_h__