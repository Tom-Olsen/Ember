#ifndef __INCLUDE_GUARD_lighting_h__
#define __INCLUDE_GUARD_lighting_h__
#include "emberMath.h"



namespace emberEngine
{
	class ShadowCascade;
	class Lighting
	{
	public: // Enums and light types:
		enum class ShadowType { hard, soft };
		struct DirectionalLight
		{
			Float3 direction;
			float intensity;
			Float3 color;
			ShadowType shadowType;

			// Shadow cascade settings:
			std::array<ShadowCascade*, 4> shadowCascades;
			float distributionFactor;		// 0 = linear, 1 = logarithmic
			uint8_t shadowCascadeCount;
			float shadowCascadeSplits[5];	// Percentile splits for each shadow cascade € [0,1].
		};
		struct PositionalLight
		{
			Float3 position;
			float intensity;
			Float3 color;
			ShadowType shadowType;

			float fov;
			float nearClip;
			float farClip;
			float blendStart;
			float blendEnd;
			Float4x4 viewMatrix;
			Float4x4 projectionMatrix;
		};

	public:	// Constexpr members:
		// When changing these also change the corresponding GPU values in shaders/shadowMapping.hlsli.
		static constexpr uint32_t maxDirectionalLights = 3;
		static constexpr uint32_t maxPositionalLights = 30;
		static constexpr uint32_t shadowMapResolution = 4096;

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
		static void SetActiveCamera();
		//static void AddDirectionalLight(float intensity, const Float3& color, ShadowType shadowType, );
		static void AddPositionalLight
		(Float3 position, float intensity, const Float3& color, ShadowType shadowType,
		 float fov, float newClip, float farClip, float blendStart, float blendEnd,
		 const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		static void ResetLights();

		// Getters:
		static uint32_t GetDirectionalLightsCount();
		static uint32_t GetPositionalLightsCount();
		static std::array<DirectionalLight, maxDirectionalLights>& GetDirectionalLights();
		static std::array<PositionalLight, maxPositionalLights>& GetPositionalLights();
		static Float4x4 GetPointLightRotationMatrix(uint32_t faceIndex);

	private: // Methods:
		// Delete all constructors:
		Lighting() = delete;
		Lighting(const Lighting&) = delete;
		Lighting& operator=(const Lighting&) = delete;
		~Lighting() = delete;
	};
}



#endif // __INCLUDE_GUARD_lighting_h__