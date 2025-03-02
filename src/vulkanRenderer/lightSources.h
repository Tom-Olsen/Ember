#ifndef __INCLUDE_GUARD_lightSources_h__
#define __INCLUDE_GUARD_lightSources_h__
#include "emberMath.h"



namespace vulkanBackend
{
	struct SpotLight
	{
		float intensity;
		Float3 color;
		ShadowType shadowType;
		float fov;
		float aspectRatio;
		float nearClip;
		float farClip;
		float blendStart;
		float blendEnd;
		Float4x4 projectionMatrix;
	};
	
	struct PointLight
	{
		float intensity;
		Float3 color;
		ShadowType shadowType;
		float nearClip;
		float farClip;
		Float4x4 projectionMatrix;
		static Float4x4 rotationMatrices[6];
	};
	
	struct DirectionalLight
	{
		float intensity;
		Float3 color;
		ShadowType shadowType;
	
		// Shadow cascade settings:
		std::array<ShadowCascade*, 4> shadowCascades;
		Camera* pActiveCamera;
		float distributionFactor;		// 0 = linear, 1 = logarithmic
		uint32_t shadowCascadeCount;
		float shadowCascadeSplits[5];	// Percentile splits for each shadow cascade € [0,1].
	};
}



#endif // __INCLUDE_GUARD_lightSources_h__