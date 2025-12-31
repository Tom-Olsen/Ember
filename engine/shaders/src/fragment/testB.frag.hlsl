//SamplerState colorSampler : register(s10);
//TextureCube cubeMap : register(t20);
//
//
//
//struct FragmentInput
//{
//    float4 position : SV_POSITION;
//    float4 uv : TEXCOORD0;
//};
//
//
//
//float4 main(FragmentInput input) : SV_TARGET
//{
//    float2 uv = input.uv.xy;
//    return cubeMap.Sample(colorSampler, normalize(float3(uv, 1.0f)));
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