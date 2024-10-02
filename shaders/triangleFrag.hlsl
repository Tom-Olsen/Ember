Texture2D texture : register(t1);
SamplerState samplerState : register(s2);



struct FragmentInput
{
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};



float4 main(FragmentInput input) : SV_TARGET
{
    float4 color = texture.Sample(samplerState, input.uv.xy); // * input.vertexColor;
    return color;
    //return input.vertexColor;
}