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
    float3 position : POSITION;
    float4 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD0;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0f);
    float4x4 localToClipMatrix = mul(cb_worldToClipMatrix, cb_localToWorldMatrix);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    output.uv = input.uv;
    return output;
}