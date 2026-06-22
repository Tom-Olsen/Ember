#include "fragmentShaderCommon.hlsli"
Texture2D renderTexture : register(t100, SHADER_SET);
Texture2D gizmoTexture : register(t101, SHADER_SET);



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    float4 renderColor = renderTexture.Sample(colorSampler, input.uv.xy);
    float4 gizmoColor = gizmoTexture.Sample(colorSampler, input.uv.xy);
    return float4(gizmoColor.rgb + renderColor.rgb * (1.0f - gizmoColor.a), gizmoColor.a + renderColor.a * (1.0f - gizmoColor.a));
}