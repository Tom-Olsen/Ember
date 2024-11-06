SamplerState colorSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);
Texture2D colorTexture : register(t3);
Texture2D shadowMap : register(t4);



#define MAX_D_LIGHTS 2
struct DLight
{
    float4x4 viewMatrix;        // light world to local matrix
    float4x4 projMatrix;        // light projection matrix
    float4x4 worldToClipMatrix; // world to light clip space matrix
    float4 directionIntensity;  // direction (xyz) and intensity (w) of the light
    float4 color;               // light color
};
cbuffer LightData : register(b5)
{
    float4x4 viewMatrix; // light world to local matrix
    float4x4 projMatrix; // light projection matrix
    float4x4 worldToClipMatrix; // world to light clip space matrix
    float4 directionIntensity; // direction (xyz) and intensity (w) of the light
    float4 color; // light color
    //DLight dLights[MAX_D_LIGHTS];
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



float Shadow(float3 worldPosition)
{
    float4x4 lightViewMatrix = float4x4
    (0.447214, -7.45058e-08, -0.894427, -3.19872e-07,
    -0.596285, 0.745356, -0.298143, 4.26496e-07,
    -0.596285, 0.666667, 0.333333, -9,
    -0, 0, -0, 1);
    float4x4 lightProjectionMatrix = float4x4
    ( 0.133333, 0, 0, -0,
      0, 0.133333, 0, -0,
      0, 0, -0.133422, -1.00133,
      0, 0, 0, 1);
    //float4 lightSpacePos = mul(lightProjectionMatrix, mul(lightViewMatrix, float4(worldPosition, 1.0f)));
    
    float4 lightSpacePos = mul(worldToClipMatrix, float4(worldPosition, 1.0f));
    //float4 lightSpacePos = mul(dLights[0].projMatrix, mul(dLights[0].viewMatrix, float4(worldPosition, 1.0f)));

    // Perform perspective divide to get normalized device coordinates (NDC):
    float depthNDC = 0.0f;
    if (lightSpacePos.w != 0.0f)
        depthNDC = lightSpacePos.z / lightSpacePos.w;

    // Map NDC depth to [0, 1] for depth comparison:
    float referenceDepth = (depthNDC * 0.5f) + 0.5f;
    
    // Get shadow value:
    float2 shadowCoords = 0.0f;
    if (lightSpacePos.w != 0.0f)
        shadowCoords = lightSpacePos.xy / lightSpacePos.w * 0.5 + 0.5;
    
    return shadowMap.SampleCmp(shadowSampler, shadowCoords, referenceDepth);
}



float4 main(FragmentInput input) : SV_TARGET
{
    float2 uv = input.uv.xy;
    float3 normal = normalize(input.normal);
    float3 color = colorTexture.Sample(colorSampler, uv).xyz;
    
    float shadow = Shadow(input.worldPos);
    
    return shadow * float4(color, 1.0f);
}