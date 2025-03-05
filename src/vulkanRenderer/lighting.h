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
			float intensity;
			Float3 color;
			ShadowType shadowType;
			Float3 direction;

			// Shadow cascade settings:
			std::array<ShadowCascade*, 4> shadowCascades;
			float distributionFactor;		// 0 = linear, 1 = logarithmic
			uint8_t shadowCascadeCount;
			float shadowCascadeSplits[5];	// Percentile splits for each shadow cascade € [0,1].
		};
		struct PointLight
		{
			float intensity;
			Float3 color;
			ShadowType shadowType;
			Float3 position;

			float nearClip;
			float farClip;
			Float4x4 viewMatrix[6];
			Float4x4 projectionMatrix;
		};
		struct SpotLight
		{
			float intensity;
			Float3 color;
			ShadowType shadowType;
			Float3 position;

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
		static constexpr uint32_t maxPointLights = 3;
		static constexpr uint32_t maxSpotLights = 5;
		static constexpr uint32_t shadowMapResolution = 4096;

	private: // Members:
		static bool s_isInitialized;
		static uint32_t s_directionalLightsCount;
		static uint32_t s_spotLightsCount;
		static uint32_t s_pointLightsCount;
		static std::array<DirectionalLight, maxDirectionalLights> s_directionalLights;
		static std::array<PointLight, maxPointLights> s_pointLights;
		static std::array<SpotLight, maxSpotLights> s_spotLights;
		static Float4x4 s_pointLightRotationMatrices[6];

	public: // Methods:
		static void Init();
		static void Clear();

		// Setters/Adders:
		static void SetActiveCamera();
		//static void AddDirectionalLight(float intensity, const Float3& color, ShadowType shadowType, );
		static void AddPointLight
		(float intensity, const Float3& color, ShadowType shadowType, Float3 position,
		 float nearClip, float farClip, const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		static void AddSpotLight
		(float intensity, const Float3& color, ShadowType shadowType, Float3 position,
		 float fov, float newClip, float farClip, float blendStart, float blendEnd,
		 const Float4x4& viewMatrix, const Float4x4& projectionMatrix);
		static void ResetLights();

		// Getters:
		static uint32_t GetDirectionalLightsCount();
		static uint32_t GetPointLightsCount();
		static uint32_t GetSpotLightsCount();
		static std::array<DirectionalLight, maxDirectionalLights>& GetDirectionalLights();
		static std::array<PointLight, maxPointLights>& GetPointLights();
		static std::array<SpotLight, maxSpotLights>& GetSpotLights();
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