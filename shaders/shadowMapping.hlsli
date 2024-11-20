#ifndef __INCLUDE_GUARD_shadowMapping_hlsli__
#define __INCLUDE_GUARD_shadowMapping_hlsli__



// Single shadow map:
float Shadow(float3 worldPos, float4x4 worldToClipMatrix, Texture2D shadowMap, SamplerComparisonState shadowSampler)
{
    float4 lightSpacePos = mul(worldToClipMatrix, float4(worldPos, 1.0f));
    float3 lightUvz = lightSpacePos.xyz / (lightSpacePos.w < 1e-4 ? 1.0f : lightSpacePos.w);
    lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f;
    
    float bias = 0;
    //float bias = 0.001 * (0.5f * sin(pc.time.x) + 0.5f);
    //float bias = max(0.0001, 0.0005 * dot(normal, lightDir));
    return shadowMap.SampleCmp(shadowSampler, lightUvz.xy, lightUvz.z + bias);
}



static const uint MAX_D_LIGHTS = 3;     // directional lights: sun, moon, etc.
static const uint MAX_P_LIGHTS = 5;     // point lights: candles, etc.
static const uint MAX_S_LIGHTS = 10;    // spot lights: car headlights, etc.



// Multiple shadow maps:
float3 Shadow(float3 worldPos, float3 normal, float3 color,
              float4x4 worldToClipMatrix[MAX_D_LIGHTS],
              float4 directionIntensity[MAX_D_LIGHTS],
              float3 lightColor[MAX_D_LIGHTS],
              Texture2DArray<float> shadowMaps, SamplerComparisonState shadowSampler)
{
    float3 totalLight = 0;
    
    for (int i = 0; i < MAX_D_LIGHTS; i++)
    {
        float4 lightSpacePos = mul(worldToClipMatrix[i], float4(worldPos, 1.0f));
        float3 lightUvz = lightSpacePos.xyz / (lightSpacePos.w < 1e-4 ? 1.0f : lightSpacePos.w);
        lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f;
        
        float bias = 0;
        //float bias = 0.001 * (0.5f * sin(pc.time.x) + 0.5f);
        //float bias = max(0.0001, 0.0005 * dot(normal, lightDir));
        float shadow = shadowMaps.SampleCmp(shadowSampler, float3(lightUvz.xy, i), lightUvz.z + bias);
        
        float3 lightDir = directionIntensity[i].xyz;
        float diffuse = max(dot(normal, lightDir), 0.0) * directionIntensity[i].w;
        
        totalLight += shadow * diffuse * color * lightColor[i];
    }
    
    return totalLight;
}



#endif // __INCLUDE_GUARD_shadowMapping_hlsli__