//SamplerComparisonState shadowSampler : register(s11);
//Texture2DArray<float> shadowMaps : register(t20);
//#include "shadowMapping.hlsli"
//#include "defaultPushConstant.hlsli"
//
//
//
//cbuffer SurfaceProperties : register(b2)
//{
//    float4 diffuseColor;    // (1.0, 1.0, 1.0)
//    float roughness;        // 0.5
//    float3 reflectivity;    // 0.4
//    float metallicity;      // 0 = dielectric, 1 = metal
//    float4 scaleOffset;     // .xy = scale, .zw = offset
//};
//
//
//
//struct FragmentInput
//{
//    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
//    float3 worldNormal : NORMAL;        // normal in world space
//    float4 vertexColor : COLOR;         // vertex color
//    float3 worldPos : TEXCOORD0;        // position in world space
//};
//
//
//
//float4 main(FragmentInput input) : SV_TARGET
//{
//    // Mesh data:
//    float4 color = diffuseColor * input.vertexColor;
//    float3 worldNormal = normalize(input.worldNormal);
//    float3 worldPos = input.worldPos;
//    
//    // Lighting:
//    float ambient = 0.1f;
//    float3 finalColor = ambient * color.xyz;
//    finalColor += PhysicalLighting(worldPos, pc.cameraPosition.xyz, worldNormal, color.xyz, roughness, reflectivity, metallicity, pc.dLightsCount, pc.sLightsCount, directionalLightData, positionalLightData, shadowMaps, shadowSampler);
//    
//    return float4(finalColor, 1.0f);
//}
struct FragmentInput
{
    float4 clipPosition : SV_POSITION; // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL; // normal in world space
    float3 worldTangent : TANGENT; // tangent in world space
    float4 vertexColor : COLOR; // vertex color
    float4 uv : TEXCOORD0; // texture coordinates
    float3 worldPosition : TEXCOORD1; // position in world space
};
float4 main(FragmentInput input) : SV_TARGET
{
    return 1;
}