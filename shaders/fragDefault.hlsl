Texture2D texture : register(t1);
SamplerState samplerState : register(s2);



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
};

float3 NegativeColor(float3 color)
{
    float3 result = color;
    if (color.x < 0)
    {
        result.x += color.x;
        result.y -= color.x;
        result.z -= color.x;
    }
    if (color.y < 0)
    {
        result.x -= color.y;
        result.y += color.y;
        result.z -= color.y;
    }
    if (color.z < 0)
    {
        result.x -= color.z;
        result.y -= color.z;
        result.z += color.z;
    }
    return result;
}

float4 main(FragmentInput input) : SV_TARGET
{
    //return input.vertexColor;
    float3 normal = normalize(input.normal);
    float3 rescaledNormal = NegativeColor(normal);
    
    float4 tex = texture.Sample(samplerState, input.uv.xy);
    return tex;
    float4 color = float4(normal * tex.rgb, tex.a);
    //float4 color = float4(rescaledNormal * tex.rgb, tex.a);

    //return float4(normal, 1.0f);
    return color;
}