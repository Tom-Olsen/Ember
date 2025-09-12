#ifndef __INCLUDE_GUARD_vulkanRendererBackend_lighting_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_lighting_h__
#include "emberMath.h"
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
			Float3 direction;
			float intensity;
			Float3 color;
			ShadowType shadowType;
			Float4x4 worldToClipMatrix;
			std::string ToString();
		};
		struct PositionalLight
		{
			Float3 position;
			float intensity;
			Float3 color;
			ShadowType shadowType;
			float blendStart;
			float blendEnd;
			Float4x4 worldToClipMatrix;
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
		static Float4x4 s_pointLightRotationMatrices[6];

	public: // Methods:
		static void Init(uint32_t maxDirectionalLights, uint32_t maxPositionalLights, uint32_t shadowMapResolution);
		static void Clear();

		// Setters/Adders:
		static void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, ShadowType shadowType, const Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const Float3& position, float intensity, const Float3& color, ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix);

		// Getters:
		static uint32_t GetMaxDirectionalLights();
		static uint32_t GetMaxPositionalLights();
		static uint32_t GetShadowMapResolution();
		static uint32_t GetDirectionalLightsCount();
		static uint32_t GetPositionalLightsCount();
		static std::vector<DirectionalLight>& GetDirectionalLights();
		static std::vector<PositionalLight>& GetPositionalLights();
		static Float4x4 GetPointLightRotationMatrix(uint32_t faceIndex);

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