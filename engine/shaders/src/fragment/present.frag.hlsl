SamplerState colorSampler : register(s10);
Texture2D renderTexture : register(t21);



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    return renderTexture.Sample(colorSampler, input.uv.xy);
}