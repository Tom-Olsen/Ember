struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

float4 main(FragmentInput input) : SV_TARGET
{
    float4 color = input.color;
    float3 normal = normalize(input.normal);
    float3 lightDir = float3(-0.577f, -0.577f, -0.577f);
    
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.2f;
    
    return (ambient + diffuse) * color;
}