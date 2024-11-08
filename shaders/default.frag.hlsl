SamplerState colorSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);
Texture2D colorTexture : register(t3);
Texture2D shadowMap : register(t4);



cbuffer LightData : register(b5)
{
    float4x4 viewMatrix;        // light world to local matrix
    float4x4 projMatrix;        // light projection matrix
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view)
    float4 directionIntensity;  // direction (xyz) and intensity (w) of the light
    float4 lightColor;          // light color
};



struct PushConstant
{
    float4 time;
    float4 delaTime;
};
#if defined(_DXC)
[[vk::push_constant]] CullPushConstants pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif



struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 vertexColor : COLOR;
    float4 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};



float Shadow(float3 worldPos)
{
    float4 lightSpacePos = mul(worldToClipMatrix, float4(worldPos, 1.0f));
    float3 lightUvz = lightSpacePos.xyz / (lightSpacePos.w < 1e-4 ? 1.0f : lightSpacePos.w);
    lightUvz.xy = (lightUvz.xy + 1.0f) * 0.5f;
    
    float bias = 0;
    //float bias = 0.001 * (0.5f * sin(pc.time.x) + 0.5f);
    //float bias = max(0.0001, 0.0005 * dot(normal, lightDir));
    return shadowMap.SampleCmp(shadowSampler, lightUvz.xy, lightUvz.z + bias);
}



float4 main(FragmentInput input) : SV_TARGET
{
    // Mesh data:
    float2 uv = input.uv.xy;
    float3 normal = normalize(input.normal);
    float3 worldPos = input.worldPos;
    
    // Lighting:
    float3 lightDir = directionIntensity.xyz;
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.1f;
    float shadow = Shadow(worldPos);
 
    // Shading:
    float4 color = colorTexture.Sample(colorSampler, uv);
    
    return (ambient + diffuse * shadow) * color;
}