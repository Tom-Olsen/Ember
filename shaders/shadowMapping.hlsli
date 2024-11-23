#ifndef __INCLUDE_GUARD_shadowMapping_hlsli__
#define __INCLUDE_GUARD_shadowMapping_hlsli__



static const uint MAX_D_LIGHTS = 3;     // directional lights: sun, moon, etc.
static const uint MAX_P_LIGHTS = 5;     // point lights: candles, etc.
static const uint MAX_S_LIGHTS = 10;    // spot lights: car headlights, etc.



struct DirectionalLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view)
    float3 direction;           // light direction
    float4 colorIntensity;      // light color (xyz) and intensity (w)
    float nearClip;
};
struct SpotLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view)
    float3 direction;           // light direction
    float4 colorIntensity;      // light color (xyz) and intensity (w)
    float nearClip;
    float2 blendStartEnd;
};
cbuffer LightData : register(b1)
{
    DirectionalLightData directionalLightData[MAX_D_LIGHTS];
    SpotLightData spotLightData[MAX_S_LIGHTS];
    bool receiveShadows; // 0 = false, 1 = true
}



float3 DirectionalShadows(float3 worldPos, float3 normal, float3 color, int dLightsCount,
              DirectionalLightData lightData[MAX_D_LIGHTS],
              Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    
    for (uint i = 0; i < dLightsCount; i++)
    {
        float shadow = 1.0f;
        if (receiveShadows)
        {
            float4 lightSpacePos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f));
            float3 lightUvz = lightSpacePos.xyz / (lightSpacePos.w < 1e-4 ? 1.0f : lightSpacePos.w);
            lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f;
            lightUvz.z = max(lightUvz.z, lightData[i].nearClip);
            shadow = shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, i), lightUvz.z);
        }
        
        float diffuse = max(dot(normal, lightData[i].direction), 0.0) * lightData[i].colorIntensity.w;
        totalLight += shadow * diffuse * color * lightData[i].colorIntensity.xyz;
    }
    
    return totalLight;
}
float3 SpotShadows(float3 worldPos, float3 normal, float3 color, int sLightsCount,
              SpotLightData lightData[MAX_S_LIGHTS],
              Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    
    for (uint i = 0; i < sLightsCount; i++)
    {
        float shadow = 1.0f;
        if (receiveShadows)
        {
            float4 lightSpacePos = mul(lightData[i].worldToClipMatrix, float4(worldPos, 1.0f));
            float3 lightUvz = lightSpacePos.xyz / (lightSpacePos.w < 1e-4 ? 1.0f : lightSpacePos.w);
            lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f;
            lightUvz.z = max(lightUvz.z, lightData[i].nearClip);
        
            float radius = length(2.0f * lightUvz.xy - 1.0f);
            float falloff = saturate((radius - lightData[i].blendStartEnd.y) / (lightData[i].blendStartEnd.x - lightData[i].blendStartEnd.y));
        
            shadow = falloff * shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, i), lightUvz.z);
        }
        
        float diffuse = max(dot(normal, lightData[i].direction), 0.0) * lightData[i].colorIntensity.w;
        totalLight += shadow * diffuse * color * lightData[i].colorIntensity.xyz;
    }
    
    return totalLight;
}



#endif // __INCLUDE_GUARD_shadowMapping_hlsli__