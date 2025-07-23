SamplerState colorSampler : register(s10);
Texture2D colorMap : register(t20);



struct FragmentInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    return colorMap.Sample(colorSampler, uv);
}