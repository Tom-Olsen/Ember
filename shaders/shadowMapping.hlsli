#ifndef __INCLUDE_GUARD_shadowMapping_hlsli__
#define __INCLUDE_GUARD_shadowMapping_hlsli__
#include "mathf.hlsli"



static const uint MAX_D_LIGHTS = 3;     // directional lights: sun, moon, etc.
static const uint MAX_S_LIGHTS = 10;    // spot lights: car headlights, etc.
static const uint MAX_P_LIGHTS = 5;     // point lights: candles, etc.



struct DirectionalLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view)
    float3 direction;           // light direction
    float4 colorIntensity;      // light color (xyz) and intensity (w)
};
struct SpotLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view)
    float3 position;            // light position
    float4 colorIntensity;      // light color (xyz) and intensity (w)
    float2 blendStartEnd;
};
struct PointLightData
{
    float4x4 worldToClipMatrix[6]; // world to light clip space matrix (projection * view)
    float3 position; // light position
    float4 colorIntensity; // light color (xyz) and intensity (w)
};
cbuffer LightData : register(b9)
{
    DirectionalLightData directionalLightData[MAX_D_LIGHTS];
    SpotLightData spotLightData[MAX_S_LIGHTS];
    PointLightData pointLightData[MAX_P_LIGHTS];
    bool receiveShadows; // 0 = false, 1 = true
}



// Helper Functions:
float NormalDistribution(float nDotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH2 = nDotH * nDotH;
    float denom = nDotH2 * (a2 - 1.0f) + 1.0f;
    return a2 / (mathf_PI * denom * denom);
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
 float3 color, float roughness, float3 reflectivity, bool metallic)
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
    float3 diffuseBRDF = metallic ? 0 : (1.0f - F) * color * mathf_PI_INV;
        
    return (diffuseBRDF + specularBRDF) * lightIntensity * nDotL;
}



float3 PhysicalDirectionalLights
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, bool metallic,
 int dLightsCount, int shadowMapOffset, DirectionalLightData lightData[MAX_D_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    for (uint i = 0; i < dLightsCount; i++)
    {
        // Shadow:
        float shadow = 0.0f;
        if (receiveShadows)
        {
            float4 lightSpacePos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f));
            if (lightSpacePos.w > 1e-4f)
            {
                float3 lightUvz = lightSpacePos.xyz / lightSpacePos.w;
                lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f; // scale to [0, 1]
                if (0.0f <= lightUvz.z && lightUvz.z <= 1.0f && 0.0 <= lightUvz.x && lightUvz.x <= 1.0 && 0.0 <= lightUvz.y && lightUvz.y <= 1.0)
                    shadow = shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, i + shadowMapOffset), lightUvz.z - 0.01f);
            }
        }
        
        // Light:
        float3 lightIntensity = lightData[i].colorIntensity.xyz * lightData[i].colorIntensity.w;
        float3 lightDir = normalize(lightData[i].direction);
        float3 viewDir = normalize(cameraPos - worldPos);
        float3 light = PhysicalLight(lightIntensity, lightDir, normal, viewDir, color, roughness, reflectivity, metallic);
        
        totalLight += shadow * light;
    }
    return totalLight;
}
float3 PhysicalSpotLights
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, bool metallic,
 int sLightsCount, int shadowMapOffset, SpotLightData lightData[MAX_S_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    for (uint i = 0; i < sLightsCount; i++)
    {
        // Shadow:
        float shadow = 0.0f;
        if (receiveShadows)
        {
            float4 lightSpacePos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f));
            if (lightSpacePos.w > 1e-4f)
            {
                float3 lightUvz = lightSpacePos.xyz / lightSpacePos.w;
                lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f; // scale to [0, 1]
                
                float radius = length(2.0f * lightUvz.xy - 1.0f);
                float falloff = saturate((radius - lightData[i].blendStartEnd.y) / (lightData[i].blendStartEnd.x - lightData[i].blendStartEnd.y));
                
                if (0.0f <= lightUvz.z && lightUvz.z <= 1.0f)
                    shadow = falloff * shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, i + shadowMapOffset), lightUvz.z);
            }
        }
        
        // Light:
        float distSq = dot(lightData[i].position - worldPos, lightData[i].position - worldPos);
        float3 lightIntensity = lightData[i].colorIntensity.xyz * lightData[i].colorIntensity.w / distSq;
        float3 lightDir = normalize(lightData[i].position - worldPos);
        float3 viewDir = normalize(cameraPos - worldPos);
        float3 light = PhysicalLight(lightIntensity, lightDir, normal, viewDir, color, roughness, reflectivity, metallic);
        
        totalLight += shadow * light;
    }
    return totalLight;
}
float3 PhysicalPointLights
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, bool metallic,
 int pLightsCount, int shadowMapOffset, PointLightData lightData[MAX_P_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    for (uint i = 0; i < pLightsCount; i++)
    {
        for (uint faceIndex = 0; faceIndex < 6; faceIndex++)
        {
            // Shadow:
            float shadow = 0.0f;
            if (receiveShadows)
            {
                float4 lightSpacePos = mul(lightData[i].worldToClipMatrix[faceIndex], float4(worldPos, 1.0f));
                if (lightSpacePos.w > 1e-4f)
                {
                    float3 lightUvz = lightSpacePos.xyz / lightSpacePos.w;
                    lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f; // scale to [0, 1]
                    if (0.0f <= lightUvz.z && lightUvz.z <= 1.0f)
                        shadow = shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, 6 * i + faceIndex + shadowMapOffset), lightUvz.z);
                }
            }
        
            // Light:
            float distSq = dot(lightData[i].position - worldPos, lightData[i].position - worldPos);
            float3 lightIntensity = lightData[i].colorIntensity.xyz * lightData[i].colorIntensity.w / distSq;
            float3 lightDir = normalize(lightData[i].position - worldPos);
            float3 viewDir = normalize(cameraPos - worldPos);
            float3 light = PhysicalLight(lightIntensity, lightDir, normal, viewDir, color, roughness, reflectivity, metallic);
        
            totalLight += shadow * light;
        }
    }
    return totalLight;
}



float3 PhysicalLighting
(float3 worldPos, float3 cameraPos, float3 normal, float3 color, float roughness, float3 reflectivity, bool metallic,
 int dLightsCount, int sLightsCount, int pLightsCount, DirectionalLightData directionalLightData[MAX_D_LIGHTS], SpotLightData spotLightData[MAX_S_LIGHTS], PointLightData pointLightData[MAX_P_LIGHTS],
 Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 directionalLight = PhysicalDirectionalLights(worldPos, cameraPos, normal, color, roughness, reflectivity, metallic, dLightsCount, 0                          , directionalLightData, shadowMaps, shadowSampler);
    float3 spotLight        = PhysicalSpotLights       (worldPos, cameraPos, normal, color, roughness, reflectivity, metallic, sLightsCount, dLightsCount               , spotLightData       , shadowMaps, shadowSampler);
    float3 pointLight       = PhysicalPointLights      (worldPos, cameraPos, normal, color, roughness, reflectivity, metallic, pLightsCount, dLightsCount + sLightsCount, pointLightData      , shadowMaps, shadowSampler);
    return directionalLight + spotLight + pointLight;
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