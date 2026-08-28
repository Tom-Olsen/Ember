#include "deferredRenderingConstants.h"
#include "shadowMapping.hlsli"



Texture2D<float4> gbufferAlbedo : register(t100, SHADER_SET);
Texture2D<float4> gbufferNormal : register(t101, SHADER_SET);
Texture2D<float4> gbufferMaterial : register(t102, SHADER_SET);
Texture2D<float> gbufferDepth : register(t103, SHADER_SET);



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
    float4 materialData = gbufferMaterial.Load(int3(pixelPosition, 0));

	// Surface properties:
    float3 albedo = albedoData.rgb;
    float3 worldNormal = normalize(normalData.rgb * 2.0f - 1.0f);
    float metallicity = materialData[DEFERRED_MATERIAL_METALLICITY_CHANNEL];
    float roughness = materialData[DEFERRED_MATERIAL_ROUGHNESS_CHANNEL];
    uint flagBitMask = uint(round(materialData[DEFERRED_MATERIAL_FLAG_BIT_MASK_CHANNEL] * DEFERRED_MATERIAL_FLAG_BIT_MASK_MAX));
    bool receiveShadows = (flagBitMask & DEFERRED_MATERIAL_FLAG_RECEIVE_SHADOWS) != 0;

	// Lighting:
	float3 worldPosition = ReconstructWorldPosition(pixelPosition, depth);
    float3 lighting = PhysicalLighting(worldPosition, worldNormal, albedo, roughness, metallicity, receiveShadows);
    return float4(lighting, 1.0f);
}