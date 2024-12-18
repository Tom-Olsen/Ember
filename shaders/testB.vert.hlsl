cbuffer RenderMatrizes : register(b0)
{
    float4x4 modelMatrix; // mesh local to world matrix
    float4x4 viewMatrix; // camera world to local matrix
    float4x4 projMatrix; // camera projection matrix
    float4x4 normalMatrix; // rotation matrix for normals and directions
    float4x4 localToClipMatrix; // local to clip space matrix: (model * view * projection)
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
    VertexOutput output;
    output.position = mul(localToClipMatrix, float4(input.position, 1.0));
    output.uv = input.uv;
    return output;
}