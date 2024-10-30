Texture2D texture : register(t1);
SamplerState samplerState : register(s2);



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};

float4 main(FragmentInput input) : SV_TARGET
{
    float depth = texture.Sample(samplerState, input.uv.xy).r;
    float value = (depth - 0.1f) / (100.0f - 0.1f);
    float4 color = value * float4(1.0f, 1.0f, 1.0f, 1.0f);
    return color;
}