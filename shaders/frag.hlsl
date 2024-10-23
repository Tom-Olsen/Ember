Texture2D texture : register(t1);
SamplerState samplerState : register(s2);



struct FragmentInput
{
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    //return input.vertexColor;
    float4 normal = float4(normalize(input.normal), 1.0f);
    return normal;
    float4 color = texture.Sample(samplerState, input.uv.xy); // * input.vertexColor;
    return color;
    //return input.vertexColor;
}