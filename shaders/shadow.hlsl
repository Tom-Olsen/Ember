cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix;   // mesh local to world matrix
    float4x4 viewMatrix;    // camera world to local matrix
    float4x4 projMatrix;    // camera projection matrix
    float4x4 normalMatrix;  // rotation matrix for normals and directions
    float4x4 mvpMatrix;     // local to clip space matrix: (model * view * projection)
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
    output.position = mul(mvpMatrix, pos);
    return output;
}