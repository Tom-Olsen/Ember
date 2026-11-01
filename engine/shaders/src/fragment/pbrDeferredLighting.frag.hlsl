#include "deferredRenderingConstants.h"
#include "shadowMapping.hlsli"



// Bindings:
Texture2D<float4> gbufferAlbedo : register(t100, SHADER_SET);
Texture2D<float4> gbufferNormal : register(t101, SHADER_SET);
Texture2D<float4> gbufferSurfaceProperties : register(t102, SHADER_SET);
Texture2D<float> gbufferDepth : register(t103, SHADER_SET);
cbuffer Settings : register(b300, SHADER_SET)
{
    int renderMode;	// 0 = Normal lighting, 1 = albedo, 2 = world normals, 3 = metallicity, 4 = roughnes, 5 = ambient occlusion, 6... maps to 0
};
static const uint renderMode_Lighting = 0;
static const uint renderMode_Albedo = 1;
static const uint renderMode_Normals = 2;
static const uint renderMode_Metallicity = 3;
static const uint renderMode_Roughness = 4;
static const uint renderMode_AmbientOcclusion = 5;



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;
};



float3 ReconstructWorldPosition(int2 pixelPosition, float depth)
{
    uint width;
    uint height;
    gbufferDepth.GetDimensions(width, height);

	float2 pixelCenter = float2(pixelPosition) + 0.5f;	// move from bottom left corner of pixel to center.
    float2 uv = pixelCenter / float2(width, height);	// in [0,1].
    float4 clipPosition = float4(2.0f * uv - 1.0f, depth, 1.0f);
    float4 worldPosition = mul(camera_clipToWorldMatrix, clipPosition);
    return worldPosition.xyz / worldPosition.w;
}



float4 main(FragmentInput input) : SV_TARGET
{
    int2 pixelPosition = int2(input.clipPosition.xy); // texel coordinate in [0, width/height].

	// Load G-Buffers:
    float depth = gbufferDepth.Load(int3(pixelPosition, 0));
    if (depth >= 1.0f)
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 albedoData = gbufferAlbedo.Load(int3(pixelPosition, 0));
    float4 normalData = gbufferNormal.Load(int3(pixelPosition, 0));
    float4 surfacePropertiesData = gbufferSurfaceProperties.Load(int3(pixelPosition, 0));

	// Surface properties:
    float3 albedo = albedoData.rgb;
    float3 worldNormal = normalize(normalData.rgb * 2.0f - 1.0f);
    float metallicity = surfacePropertiesData[DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL];
    float roughness = surfacePropertiesData[DEFERRED_SURFACE_PROPERTIES_ROUGHNESS_CHANNEL];
    float ambientOcclusion = surfacePropertiesData[DEFERRED_SURFACE_PROPERTIES_AMBIENT_OCCLUSION_CHANNEL];
    uint flagBitMask = uint(round(surfacePropertiesData[DEFERRED_SURFACE_PROPERTIES_FLAG_BIT_MASK_CHANNEL] * DEFERRED_SURFACE_FLAG_BIT_MASK_MAX));
    bool receiveShadows = (flagBitMask & DEFERRED_SURFACE_FLAG_RECEIVE_SHADOWS) != 0;
	bool lit = (flagBitMask & DEFERRED_SURFACE_FLAG_LIT) != 0;

	// Debugging render modes:
	switch (renderMode)
	{
		case renderMode_Albedo:
			return float4(albedo, 1.0f);
			break;
		case renderMode_Normals:
			return float4(worldNormal, 1.0f);
			break;
		case renderMode_Metallicity:
			return float4(metallicity.xxx, 1.0f);
			break;
		case renderMode_Roughness:
			return float4(roughness.xxx, 1.0f);
			break;
		case renderMode_AmbientOcclusion:
			return float4(ambientOcclusion.xxx, 1.0f);
			break;
		case renderMode_Lighting:
			break;
	}

	// Default render mode:
	if (lit)
	{
		// Lighting:
		float3 worldPosition = ReconstructWorldPosition(pixelPosition, depth);
        float3 lighting = PhysicalLighting(worldPosition, worldNormal, albedo, roughness, metallicity, receiveShadows, ambientOcclusion);
    	return float4(lighting, 1.0f);
	}
	else
		return float4(albedo, 1.0f);
}