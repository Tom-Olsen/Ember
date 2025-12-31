//#include "defaultPushConstant.hlsli"
//
//
//
//cbuffer SurfaceProperties : register(b2)
//{
//    float4 diffuseColor;    // (1.0, 1.0, 1.0)
//    float roughness;        // 0.5
//    float3 reflectivity;    // 0.4
//    bool metallic;
//    float4 scaleOffset;     // .xy = scale, .zw = offset
//};
//
//
//
//struct FragmentInput
//{
//    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
//    float4 vertexColor : COLOR;         // vertex color
//};
//
//
//
//float4 main(FragmentInput input) : SV_TARGET
//{
//    return diffuseColor * input.vertexColor;
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