#include "descriptorSetMacros.h"
#include "defaultPushConstant.hlsli"
#include "math.hlsli"
#include "vertexShaderCommon.hlsli"



struct VertexInput
{
    uint instanceID : SV_InstanceID;    // Instance ID: System value => built in variable
    float3 position : POSITION;         // position in local/model sapce
    float3 normal : NORMAL;             // normal in local/model space
    float3 tangent : TANGENT;           // tangent in local/model space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;
    float test[2] : TEST;
};
struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
    float test[2] : TEST;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    float4 tangent = float4(input.tangent, 0.0f);
    float4x4 localToWorldMatrix = model_localToWorldMatrix;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        localToWorldMatrix = mul(model_localToWorldMatrix, instanceBuffer[input.instanceID].localToWorldMatrix);
    float4x4 localToClipMatrix = mul(camera_worldToClipMatrix, localToWorldMatrix);
    float4x4 normalMatrix = LinAlg_NormalMatrix(localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.worldNormal = mul(normalMatrix, normal).xyz;
    output.worldTangent = mul(normalMatrix, tangent).xyz;
    output.vertexColor = input.vertexColor;
    if (pc.instanceCount != 0 && input.instanceID < pc.instanceCount)
        output.vertexColor *= instanceBuffer[input.instanceID].color;
    output.uv = input.uv;
    output.worldPosition = mul(localToWorldMatrix, pos).xyz;
    output.test = input.test;
    return output;
}