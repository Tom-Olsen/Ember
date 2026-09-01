#include "deferredRenderingConstants.h"
#include "fragmentShaderCommon.hlsli"
#include "pbrSurfaceProperties.hlsli"



Texture2D<float4> colorMap : register(t100, SHADER_SET);			// format = VK_FORMAT_R8G8B8A8_SRGB
Texture2D<float4> normalMap : register(t101, SHADER_SET);			// format = VK_FORMAT_R8G8B8A8_UNORM, OpenGL-style normal map
Texture2D<float> metallicityMap : register(t102, SHADER_SET);		// format = VK_FORMAT_R8_UNORM
Texture2D<float> roughnessMap : register(t103, SHADER_SET);			// format = VK_FORMAT_R8_UNORM
Texture2D<float> ambientOcclusionMap : register(t104, SHADER_SET);	// format = VK_FORMAT_R8_UNORM



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
};
struct FragmentOutput
{
    float4 albedo : SV_TARGET0;          // R8G8B8A8_SRGB
    float4 normal : SV_TARGET1;          // A2B10G10R10_UNORM_PACK32, encoded world-space normal
    float4 material : SV_TARGET2;        // R8G8B8A8_UNORM: metallicity, roughness, ambient occlusion, flags
};



FragmentOutput main(FragmentInput input)
{
    // Mesh data:
    float2 uv = input.uv.xy * surface_scaleOffset.xy + surface_scaleOffset.zw;
    float3 tangentSpaceNormal = normalize(input.worldNormal);
    float3 tangentSpaceTangent = normalize(LinAlg_VectorToPlaneProjection(input.worldTangent, tangentSpaceNormal));
    float3 tangentSpaceBitangent = cross(tangentSpaceNormal, tangentSpaceTangent);
    float3x3 tangentToWorldMatrix = transpose(float3x3(tangentSpaceTangent, tangentSpaceBitangent, tangentSpaceNormal));

    // Surface properties:
    float4 albedo = input.vertexColor * surface_diffuseColor * colorMap.Sample(colorSampler, uv);
    float3 localNormal = 2.0f * normalMap.Sample(colorSampler, uv).xyz - 1.0f;
    float3 worldNormal = normalize(mul(tangentToWorldMatrix, localNormal));
    float roughness = saturate(surface_roughness * roughnessMap.Sample(colorSampler, uv));
    float metallicity = saturate(surface_metallicity * metallicityMap.Sample(colorSampler, uv));
    float ambientOcclusion = saturate(surface_ambientOcclusion * ambientOcclusionMap.Sample(colorSampler, uv));

    // Per-pixel material flags:
    uint flagBitMask = pc.receiveShadows != 0 ? DEFERRED_MATERIAL_FLAG_RECEIVE_SHADOWS : 0u;
	flagBitMask |= surface_isLit != 0 ? DEFERRED_MATERIAL_FLAG_LIT : 0u;
    float encodedFlagBitMask = float(flagBitMask) / float(DEFERRED_MATERIAL_FLAG_BIT_MASK_MAX);

    FragmentOutput output;
    output.albedo = albedo;
    output.normal = float4(0.5f * worldNormal + 0.5f, 0.0f);
    output.material = float4(metallicity, roughness, ambientOcclusion, encodedFlagBitMask);
    return output;
}