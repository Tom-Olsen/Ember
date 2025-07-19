#ifndef __INCLUDE_GUARD_shadowMapping_hlsli__
#define __INCLUDE_GUARD_shadowMapping_hlsli__
#include "math.hlsli"
#include "shadowConstants.h"



static const float2 SHADOW_MAP_TEXEL_SIZE = float2(1.0f / SHADOW_MAP_RESOLUTION, 1.0f / SHADOW_MAP_RESOLUTION); // shadow map texel size



struct DirectionalLightData
{
    // one for each shadow cascade
    float4x4 worldToClipMatrix;     // world to light clip space matrix (projection * view).
    float3 direction;               // light direction.
    int shadowType;                 // 0 = PCF shadows, 1 = pixel shadows.
    float4 colorIntensity;          // light color (xyz) and intensity (w).
};
struct PositionalLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view).
    float3 position;            // light position.
    int shadowType;             // 0 = PCF shadows, 1 = pixel shadows.
    float4 colorIntensity;      // light color (xyz) and intensity (w).
    float2 blendStartEnd;       // start and end of fallOff in radial direction �[0,1].
};
cbuffer LightData : register(b9)
{
    DirectionalLightData directionalLightData[MAX_DIR_LIGHTS];
    PositionalLightData positionalLightData[MAX_POS_LIGHTS];
    bool receiveShadows; // 0 = false, 1 = true
}



// Helper Functions:
float NormalDistribution(float nDotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH2 = nDotH * nDotH;
    float denom = nDotH2 * (a2 - 1.0f) + 1.0f;
    return a2 / (math_PI * denom * denom);
}
float GeometryDistribution(float nDotL, float nDotV, float roughness)
{
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    float a = nDotL / (nDotL * (1.0f - k) + k);
    float b = nDotV / (nDotV * (1.0f - k) + k);
    return a * b;
}
float3 FresnelDistribution(float3 F0, float dot)
{
    return F0 + (1.0f - F0) * pow(1.0f - dot, 5.0f);
}



float3 PhysicalLight
(float3 lightIntensity, float3 lightDir, float3 normal, float3 viewDir,
 float3 color, float roughness, float3 reflectivity, float metallicity)
{
    float3 L = lightDir;
    float3 N = normal;
    float3 V = viewDir;
    float3 H = normalize(L + V);
        
    float nDotH = saturate(dot(N, H));
    float vDotH = saturate(dot(V, H));
    float nDotL = saturate(dot(N, L));
    float nDotV = saturate(dot(N, V));
        
    float D = NormalDistribution(nDotH, roughness);
    float G = GeometryDistribution(nDotL, nDotV, roughness);
    float3 F = FresnelDistribution(reflectivity, vDotH);
        
    float3 specularBRDF = (D * G * F) / (4.0f * nDotL * nDotV + 0.001f);
    float3 diffuseBRDF = (1.0f - metallicity) * (1.0f - F) * color * math_PI_INV;
        
    return (diffuseBRDF + specularBRDF) * lightIntensity * nDotL;
}

float NoFileredShadow(SamplerComparisonState shadowSampler, Texture2DArray <float>shadowMaps, int arrayIndex, float3 lightUvz)
{
    return shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, arrayIndex), lightUvz.z);
}
float PercentageCloserFilteredShadow(SamplerComparisonState shadowSampler, Texture2DArray<float> shadowMaps, int arrayIndex, float3 lightUvz)
{
    //// Average:
    //const int pcfSamples = 16;
    //float2 pcfOffsets[16] =
    //{
    //    float2(-1.5, -1.5), float2(-1.5, -0.5), float2(-1.5, 1), float2(-1.5, 1.5),
    //    float2(-0.5, -1.5), float2(-0.5, -0.5), float2(-0.5, 1), float2(-0.5, 1.5),
    //    float2( 0.5, -1.5), float2( 0.5, -0.5), float2( 0.5, 1), float2( 0.5, 1.5),
    //    float2( 1.5, -1.5), float2( 1.5, -0.5), float2( 1.5, 1), float2( 1.5, 1.5)
    //};
    //            
    //float shadow = 0.0f;
    //for (int j = 0; j < pcfSamples; j++)
    //{
    //    float2 offset = pcfOffsets[j] * SHADOW_MAP_TEXEL_SIZE;
    //    shadow += shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy + offset, arrayIndex), lightUvz.z);
    //}
    //return shadow / pcfSamples;
    
    //// Bilinear:
    //float2 texelPos = float2(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION) * lightUvz.xy;
    //float2 texelFloor = floor(texelPos);
    //float2 texelFraction = texelPos - texelFloor;
    //
    //// Sample the 4 closest texels with PCF
    //float shadow00 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(0, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    //float shadow10 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(1, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    //float shadow01 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(0, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    //float shadow11 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(1, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    //
    //// Perform bilinear interpolation
    //return Interpolation_Bilinear(texelFraction, shadow00, shadow01, shadow10, shadow11);
    
    // Bicubic:
    float2 texelPos = float2(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION) * lightUvz.xy;
    float2 texelFloor = floor(texelPos);
    float2 texelFraction = texelPos - texelFloor;
    
    // Sample the 16 closest texels with PCF
    float shadow_m1m1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(-1, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_m1p0 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(-1, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_m1p1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(-1, 2)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_m1p2 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2(-1, 3)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p0m1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 0, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p0p0 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 0, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p0p1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 0, 2)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p0p2 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 0, 3)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p1m1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 1, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p1p0 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 1, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p1p1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 1, 2)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p1p2 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 1, 3)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p2m1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 2, 0)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p2p0 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 2, 1)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p2p1 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 2, 2)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    float shadow_p2p2 = shadowMaps.SampleCmp(shadowSampler, float3((texelFloor + float2( 2, 3)) * SHADOW_MAP_TEXEL_SIZE, arrayIndex), lightUvz.z);
    
    // Perform bilinear interpolation
    return Interpolation_Bicubic(texelFraction,
    shadow_m1m1, shadow_m1p0, shadow_m1p1, shadow_m1p2,
    shadow_p0m1, shadow_p0p0, shadow_p0p1, shadow_p0p2,
    shadow_p1m1, shadow_p1p0, shadow_p1p1, shadow_p1p2,
    shadow_p2m1, shadow_p2p0, shadow_p2p1, shadow_p2p2);
}



float3 PhysicalDirectionalLights
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, float metallicity,
 int dLightsCount, int shadowMapOffset, DirectionalLightData lightData[MAX_DIR_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    for (uint i = 0; i < dLightsCount; i++)
    {
        // Check if the pixel is inside the shadow map:
        float4 lightSpaceClipPos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f)); // �[-w,w]
        float w = (abs(lightSpaceClipPos.w) < 1e-4f) ? 1e-4f : lightSpaceClipPos.w;
        float3 lightUvz = lightSpaceClipPos.xyz / w;    // ndc: xy�[-1,1] z�[0,1] (vulkan)
        lightUvz.xy = 0.5f * (lightUvz.xy + 1.0f);      // remap xy to [0,1]
        if (0.0f <= lightUvz.x && lightUvz.x <= 1.0f
         && 0.0f <= lightUvz.y && lightUvz.y <= 1.0f
         && 0.0f <= lightUvz.z && lightUvz.z <= 1.0f)
        {
            // Shadow:
            float shadow = 1.0f;
            if (receiveShadows)
            {
                if (lightData[i].shadowType == 0)
                    shadow = NoFileredShadow(shadowSampler, shadowMaps, i + shadowMapOffset, lightUvz);
                else if (lightData[i].shadowType == 1)
                    shadow = PercentageCloserFilteredShadow(shadowSampler, shadowMaps, i + shadowMapOffset, lightUvz);
            }
            
            // Light:
            float3 lightIntensity = lightData[i].colorIntensity.xyz * lightData[i].colorIntensity.w;
            float3 lightDir = normalize(-lightData[i].direction);
            float3 viewDir = normalize(cameraPos - worldPos);
            float3 light = PhysicalLight(lightIntensity, lightDir, normal, viewDir, color, roughness, reflectivity, metallicity);
        
            totalLight += shadow * light;
            break; // no need to test further shadow maps of this directional light as cascades are sorted from closest to farthest
        }
    }
    return totalLight;
}
float3 PhysicalPositionalLights
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, float metallicity,
 int sLightsCount, int shadowMapOffset, PositionalLightData lightData[MAX_POS_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    for (uint i = 0; i < sLightsCount; i++)
    {
        // Check if the pixel is inside the shadow map:
        float4 lightSpaceClipPos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f)); // �[-w,w]
        float w = (abs(lightSpaceClipPos.w) < 1e-4f) ? 1e-4f : lightSpaceClipPos.w;
        float3 lightUvz = lightSpaceClipPos.xyz / w;    // ndc: xy�[-1,1] z�[0,1] (vulkan)
        lightUvz.xy = 0.5f * (lightUvz.xy + 1.0f);      // remap xy to [0,1]
        if (0.0f <= lightUvz.x && lightUvz.x <= 1.0f
         && 0.0f <= lightUvz.y && lightUvz.y <= 1.0f
         && 0.0f <= lightUvz.z && lightUvz.z <= 1.0f)
        {
            // Shadow:
            float shadow = 1.0f;
            if (receiveShadows)
            {
                float radius = length(2.0f * lightUvz.xy - 1.0f);
                float falloff = saturate((radius - lightData[i].blendStartEnd.y) / (lightData[i].blendStartEnd.x - lightData[i].blendStartEnd.y));
                
                if (lightData[i].shadowType == 0)
                    shadow = falloff * NoFileredShadow(shadowSampler, shadowMaps, i + shadowMapOffset, lightUvz);
                else if (lightData[i].shadowType == 1)
                    shadow = falloff * PercentageCloserFilteredShadow(shadowSampler, shadowMaps, i + shadowMapOffset, lightUvz);
            }
            
            // Light:
            float distSq = dot(lightData[i].position - worldPos, lightData[i].position - worldPos);
            float3 lightIntensity = lightData[i].colorIntensity.xyz * lightData[i].colorIntensity.w / distSq;
            float3 lightDir = normalize(lightData[i].position - worldPos);
            float3 viewDir = normalize(cameraPos - worldPos);
            float3 light = PhysicalLight(lightIntensity, lightDir, normal, viewDir, color, roughness, reflectivity, metallicity);
        
            totalLight += shadow * light;
        }
    }
    return totalLight;
}



float3 PhysicalLighting
(float3 worldPos, float3 cameraPos, float3 worldNormal, float3 color, float roughness, float3 reflectivity, float metallicity,
 int dLightsCount, int sLightsCount, DirectionalLightData directionalLightData[MAX_DIR_LIGHTS], PositionalLightData positionalLightData[MAX_POS_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 directionalLight = PhysicalDirectionalLights(worldPos, cameraPos, worldNormal, color, roughness, reflectivity, metallicity, dLightsCount, 0           , directionalLightData, shadowMaps, shadowSampler);
    float3 positionalLight  = PhysicalPositionalLights (worldPos, cameraPos, worldNormal, color, roughness, reflectivity, metallicity, sLightsCount, dLightsCount, positionalLightData, shadowMaps, shadowSampler);
    return directionalLight + positionalLight;
}

// Look up tables for physically based lighting:
// Reflectivity:        linear           sRGB
// Water                0.02 0.02 0.02 | 0.15 0.15 0.15
// Plastic/Glass Low    0.03 0.03 0.03 | 0.21 0.21 0.21
// Plastic High         0.05 0.05 0.05 | 0.24 0.24 0.24
// Glass High, Ruby     0.08 0.08 0.08 | 0.31 0.31 0.31
// Diamond              0.17 0.17 0.17 | 0.45 0.45 0.45
// Iron                 0.56 0.57 0.58 | 0.77 0.78 0.78
// Copper               0.95 0.64 0.54 | 0.98 0.82 0.76
// Gold                 1.00 0.71 0.29 | 1.00 0.86 0.57
// Aluminium            0.91 0.92 0.92 | 0.96 0.96 0.97
// Silver               0.95 0.93 0.88 | 0.98 0.97 0.95



#endif // __INCLUDE_GUARD_shadowMapping_hlsli__