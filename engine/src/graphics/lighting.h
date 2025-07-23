#ifndef __INCLUDE_GUARD_lighting_h__
#define __INCLUDE_GUARD_lighting_h__
#include "emberMath.h"
#include "shadowConstants.h"
#include <string>



namespace emberEngine
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

	public:	// Constexpr members:
		static constexpr uint32_t maxDirectionalLights = MAX_DIR_LIGHTS;
		static constexpr uint32_t maxPositionalLights = MAX_POS_LIGHTS;
		static constexpr uint32_t shadowMapResolution = SHADOW_MAP_RESOLUTION;

	private: // Members:
		static bool s_isInitialized;
		static uint32_t s_directionalLightsCount;
		static uint32_t s_positionalLightsCount;
		static std::array<DirectionalLight, maxDirectionalLights> s_directionalLights;
		static std::array<PositionalLight, maxPositionalLights> s_positionalLights;
		static Float4x4 s_pointLightRotationMatrices[6];

	public: // Methods:
		static void Init();
		static void Clear();

		// Setters/Adders:
		static void AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, ShadowType shadowType, const Float4x4& worldToClipMatrix);
		static void AddPositionalLight(const Float3& position, float intensity, const Float3& color, ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix);

		// Getters:
		static uint32_t GetDirectionalLightsCount();
		static uint32_t GetPositionalLightsCount();
		static std::array<DirectionalLight, maxDirectionalLights>& GetDirectionalLights();
		static std::array<PositionalLight, maxPositionalLights>& GetPositionalLights();
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



#endif // __INCLUDE_GUARD_lighting_h__