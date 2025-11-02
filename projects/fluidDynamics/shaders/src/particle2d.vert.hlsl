#include "defaultPushConstant.hlsli"
#include "math.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 cb_localToWorldMatrix; // local to world matrix (also known as model matrix).
    float4x4 cb_viewMatrix;         // world to camera matrix.
    float4x4 cb_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix;  // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};
cbuffer Values : register(b1)
{
    float targetDensity;
    float maxVelocity;
    int colorMode;
};



StructuredBuffer<float2> positionBuffer : register(t2);
StructuredBuffer<float2> velocityBuffer : register(t3);
StructuredBuffer<float> densityBuffer : register(t4);
StructuredBuffer<float2> normalBuffer : register(t5);
StructuredBuffer<float> curvatureBuffer : register(t6);



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float3 normal : NORMAL;             // normal in local/model space
    float3 tangent : TANGENT;           // tangent in local/model space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    float4 tangent = float4(input.tangent, 0.0f);
    float4x4 localToWorldMatrix = cb_localToWorldMatrix;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        float4x4 positionMatrix = LinAlg_Translate(float3(positionBuffer[input.instanceID].xy, 0));
        localToWorldMatrix = mul(cb_localToWorldMatrix, positionMatrix);
    }
    float4x4 localToClipMatrix = mul(cb_worldToClipMatrix, localToWorldMatrix);
    float4x4 normalMatrix = LinAlg_NormalMatrix(localToWorldMatrix);
    
    float4 color = input.vertexColor;
    // Color by density:
    if (colorMode == 0 && pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        float t = (densityBuffer[input.instanceID] - targetDensity) / targetDensity;
        float t0 = clamp(t, 0.0f, 1.0f);
        float t1 = clamp(t - 1.0f, 0.0f, 1.0f);
        float4 colorA = t0 * float4(1, 1, 1, 1) + (1.0f - t0) * float4(0, 0, 1, 1);
        float4 colorB = t1 * float4(1, 0, 0, 1) + (1.0f - t1) * float4(1, 1, 1, 1);
        color *= (t < 1.0f) ? colorA : colorB;
    }
    // Color by velocity:
    if (colorMode == 1 && pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        float t = length(velocityBuffer[input.instanceID]) / maxVelocity;
        float t0 = 2.0f * t;
        float t1 = 2.0f * t - 1.0f;
        float4 colorA = t0 * float4(1, 1, 1, 1) + (1.0f - t0) * float4(0, 0, 1, 1);
        float4 colorB = t1 * float4(1, 0, 0, 1) + (1.0f - t1) * float4(1, 1, 1, 1);
        color *= (t < 0.5f) ? colorA : colorB;
    }
    // Color by normal:
    if (colorMode == 2 && pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        color *= float4(normalBuffer[input.instanceID], 0, 1);
    }
    // Color by curvature:
    if (colorMode == 3 && pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
    {
        color *= (0.5f + curvatureBuffer[input.instanceID]) * float4(0, 0, 1, 1);
    }
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.worldNormal = mul(normalMatrix, normal).xyz;
    output.worldTangent = mul(normalMatrix, tangent).xyz;
    output.vertexColor = color;
    output.uv = input.uv;
    return output;
}