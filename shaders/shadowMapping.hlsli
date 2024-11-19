#ifndef __INCLUDE_GUARD_shadowMapping_hlsli__
#define __INCLUDE_GUARD_shadowMapping_hlsli__



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



#endif // __INCLUDE_GUARD_shadowMapping_hlsli__