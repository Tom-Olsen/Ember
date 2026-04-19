#include "fragmentShaderCommon.hlsli"
Texture2D colorMap : register(t100, SHADER_SET);                // format = VK_FORMAT_R8G8B8A8_SRGB,



cbuffer SurfaceProperties : register(b300, DRAW_SET)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float4 scaleOffset;     // .xy = scale, .zw = offset
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
};



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy * scaleOffset.xy + scaleOffset.zw;
    float4 col = input.vertexColor;
    
    // Shading:
    float4 color = col * diffuseColor * colorMap.Sample(colorSampler, uv);
    
    return color;
}