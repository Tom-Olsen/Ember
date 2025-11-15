SamplerState colorSampler : register(s10);
TextureCube cubeMap : register(t20);



struct FragmentInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    return cubeMap.Sample(colorSampler, normalize(float3(uv, 1.0f)));
}