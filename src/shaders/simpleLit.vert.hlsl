#include "defaultPushConstant.hlsli"
#include "mathf.hlsli"



cbuffer RenderMatrizes : register(b0)
{
    float4x4 cb_localToWorldMatrix;    // local to world matrix (also known as model matrix).
    float4x4 cb_viewMatrix;            // world to camera matrix.
    float4x4 cb_projMatrix;            // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 cb_worldToClipMatrix;     // world to camera clip space matrix: (projection * view)
    float4x4 cb_localToClipMatrix;     // local to camera clip space matrix: (projection * view * localToWorldMatrix)
};



struct VertexInput
{
    float3 position : POSITION; // position in local/model sapce
    float3 normal : NORMAL;     // normal in local/model space
};

struct VertexOutput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldPosition : TEXCOORD1;   // position in world space
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4 normal = float4(input.normal, 0.0f);
    float4x4 localToClipMatrix = mul(cb_worldToClipMatrix, cb_localToWorldMatrix);
    float4x4 normalMatrix = LinAlg_NormalMatrix(cb_localToWorldMatrix);
    
    VertexOutput output;
    output.clipPosition = mul(localToClipMatrix, pos);
    output.worldNormal = mul(normalMatrix, normal).xyz;
    output.worldPosition = mul(cb_localToWorldMatrix, pos).xyz;
    return output;
}