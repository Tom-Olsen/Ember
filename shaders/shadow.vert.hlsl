cbuffer DirectionalLightRenderMatrizes : register(b0)
{
    float4x4 modelMatrix;       // mesh local to world matrix
    float4x4 viewMatrix;        // light world to local matrix
    float4x4 projMatrix;        // light projection matrix
    float4x4 localToClipMatrix; // local to camera clip space matrix: (projection * view * model)
};



struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};



VertexOutput main(VertexInput input)
{
    float4 pos = float4(input.position, 1.0);
    
    VertexOutput output;
    output.position = mul(localToClipMatrix, pos);
    return output;
}